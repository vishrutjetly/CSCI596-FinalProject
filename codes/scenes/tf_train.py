#!/usr/bin/env python
# ----------------------------------------------------------------------------
#
# MantaFlow fluid solver framework
# Copyright 2018 Kiwon Um, Nils Thuerey
#
# This program is free software, distributed under the terms of the
# Apache License, Version 2.0
# http://www.apache.org/licenses/LICENSE-2.0
#
# Training the MLFLIP model using TensorFlow
#
# ----------------------------------------------------------------------------

import sys, os, argparse, shutil, inspect

import numpy as np
import tensorflow.compat.v1 as tf
tf.disable_v2_behavior()

import tf_datasets, tf_network

import logging, pickle

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

parser = argparse.ArgumentParser(description='Generate Training Data', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-o', '--output', default='/tmp/tfmodel/',   help='output directory')
parser.add_argument('-l', '--load',   action="store_true",       help='load and resume the training')
parser.add_argument(      '--lstep',  default=100,   type=int,   help='log summary; e.g., every 10th steps')
parser.add_argument('-s', '--steps',  default=20000, type=int,   help='maximum training steps')
parser.add_argument('-b', '--batch',  default=5000,  type=int,   help='batch size for one step training')
parser.add_argument('-t', '--ftest',  default=0.25,  type=float, help='fraction for the test data set')
parser.add_argument('-d', '--dnet',   default='27-34-2',         help='detection networks int-int-...')
parser.add_argument('-m', '--mnet',   default='27-34-2',         help='modification networks int-int-...')
parser.add_argument(      '--dact',   default='none-tanh-tanh',  help='activation function for detection networks')
parser.add_argument(      '--mact',   default='none-tanh-tanh',  help='activation function for modification networks')
parser.add_argument(      '--stats',  action="store_true",       help='write the stats')
parser.add_argument('-v', '--mve',    action="store_true",       help='turn on mean-variance learning')
parser.add_argument(      '--nosmax', action="store_true",       help='do not use the softmax model')
parser.add_argument('-r', '--decay',  default=0.1,   type=float, help='regularization coefficient')
parser.add_argument(      '--ddrop',  default=0.1,   type=float, help='dropout rate (detection)')
parser.add_argument(      '--mdrop',  default=0.1,   type=float, help='dropout rate (modification)')
parser.add_argument('datadirs', action="store", nargs="+",       help='path(s) to the training data')
pargs = parser.parse_args()

print(sys.argv)
# exit()

pargs.output = os.path.normpath(pargs.output)
os.path.isdir(pargs.output) or os.makedirs(pargs.output)

shutil.copy(inspect.stack()[-1][1], pargs.output+'/')
with open(pargs.output+'/run_args.pickle', 'wb') as f: pickle.dump(vars(pargs), f)
with open(pargs.output+'/run_cmd.txt', 'w') as f: f.write(' '.join(os.uname()) + '\n' + ' '.join(sys.argv))

data_sets, N_tuple = tf_datasets.read_data_sets(dirs=sorted(pargs.datadirs), use_softmax=(not pargs.nosmax), frac_test=pargs.ftest)
scale = { i: max(abs(data_sets.train.get_data()[i].min()), abs(data_sets.train.get_data()[i].max())) for i in data_sets.train.get_data() }
with open(pargs.output+'/scale.pickle', 'wb') as f: pickle.dump(scale, f)

logging.basicConfig(filename='{}/training-info.log'.format(pargs.output), level=logging.INFO)
logging.getLogger().addHandler(logging.StreamHandler())
logging.info('tensorflow-{} ({}, {})'.format(tf.__version__, tf.sysconfig.get_include(), tf.sysconfig.get_lib()))
logging.info(pargs)
logging.info('{} tuples have been loaded; randomly selected {} for the training set and {} for the test set'.format(
    N_tuple, data_sets.train._num_examples, data_sets.test._num_examples))
logging.info(scale)

# statistics
if pargs.stats and (not pargs.load):
    with PdfPages(pargs.output+'/histogram.pdf') as pdf, open(pargs.output+'/dataset_stats.pickle', 'wb') as log:
        l = data_sets.train.get_data()['labels']
        if not pargs.nosmax: l = l[:,0]
        dataset_stats = {}
        for i in sorted(data_sets.train.get_data()):
            d = data_sets.train.get_data()[i][(l==1).reshape(-1)] # only splash particles
            dataset_stats[i] = [None]*d.shape[1]
            for j in range(d.shape[1]):
                d_row = d[:,j].reshape(-1)
                dataset_stats[i][j] = { 'mean': np.mean(d_row), 'std': np.std(d_row), 'min': np.amin(d_row), 'max': np.amax(d_row) }
                plt.figure()
                plt.hist(d_row, bins='auto')
                plt.title('Histogram of {}[{}]'.format(i, j))
                plt.savefig(pdf, format='pdf')
                plt.close()

        pickle.dump(dataset_stats, log)

sess = tf.InteractiveSession()

################################################################################
# neural networks
logging.info('Neural network structure: detection {} and modification {}'.format(pargs.dnet, pargs.mnet))
dlayers    = list(map(int, pargs.dnet.split('-')))
mlayers    = list(map(int, pargs.mnet.split('-')))
dact       = list(map(tf_network.parse_act, pargs.dact.split('-')))
mact       = list(map(tf_network.parse_act, pargs.mact.split('-')))
init_w     = {'w': {'stddev': 0.1}, 'b': {'value': 0.5}}
x          = tf.placeholder(tf.float32, shape=[None, dlayers[0]], name='x-input')
keep_prob  = tf.placeholder(tf.float32, name='keep_prob_detector') if pargs.ddrop>0.0 else None
keep_prob2 = tf.placeholder(tf.float32, name='keep_prob_modifier') if pargs.mdrop>0.0 else None
y_,  y     = tf_network.build_network(dlayers, dact, init_weights=init_w, input_x_holder=x, dropout_holder=keep_prob,  bn=True, scope='detector/')[1:]
y2_, y2    = tf_network.build_network(mlayers, mact, init_weights=init_w, input_x_holder=x, dropout_holder=keep_prob2, bn=True, scope='modifier/')[1:]
if pargs.mve:
    s      = tf_network.build_network(mlayers, mact, init_weights=init_w, input_x_holder=x, input_y_holder=y2_, dropout_holder=keep_prob2, bn=True, scope='modifier_var/')[2]

global_step = tf.Variable(0, trainable=False, name='global_step')

################################################################################
# evaluation functions
log_dict = {}
with tf.name_scope('accuracy'):
    with tf.name_scope('correct_prediction'):
        if pargs.nosmax: corr, appx = tf.cast(tf.less(y_, 0.5), tf.int64), tf.cast(tf.less(y, 0.5), tf.int64) # f: splashing, t: non-splashing
        else:            corr, appx = tf.argmax(y_, 1), tf.argmax(y, 1)                                       # 0: splashing, 1: non-splashing

        correct_prediction = tf.equal(corr, appx)
        accuracy           = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
        y_b_appx           = tf.equal(appx, 0) # true: splashing, false: non-splashing
        y_b_corr           = tf.equal(corr, 0)

        N_corr_non_splas    = tf.reduce_sum(corr)
        N_corr_splashing    = tf.cast(tf.shape(y_)[0], tf.int64) - N_corr_non_splas
        diff_appx_corr      = tf.logical_xor(y_b_appx, y_b_corr)
        N_corr_spl_appx_non = tf.reduce_sum(tf.cast(tf.logical_and(y_b_corr, diff_appx_corr), tf.float32))
        N_corr_non_appx_spl = tf.reduce_sum(tf.cast(tf.logical_and(y_b_appx, diff_appx_corr), tf.float32))
        false_negative      = N_corr_spl_appx_non/tf.cast(N_corr_splashing, tf.float32)
        false_positive      = N_corr_non_appx_spl/tf.cast(N_corr_non_splas, tf.float32)

        log_dict['accuracy']                     = accuracy
        log_dict['false_negative_corr_T_appx_F'] = false_negative
        log_dict['false_positive_corr_F_appx_T'] = false_positive
        log_dict['splashes/corr']                = 1.0 - tf.reduce_mean(tf.cast(corr, tf.float32))
        log_dict['splashes/appx']                = 1.0 - tf.reduce_mean(tf.cast(appx, tf.float32))

    with tf.name_scope('loss'):
        loss_normalizer = 1.0/tf.cast(tf.shape(y2_)[0], tf.float32)

        with tf.name_scope('detector'):
            if pargs.nosmax: loss_detector = tf.nn.l2_loss(y - y_)
            else:            loss_detector = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits_v2(labels=y_, logits=y))
            log_dict['detector/loss'] = loss_detector*loss_normalizer if pargs.nosmax else loss_detector

        with tf.name_scope('modifier'):
            loss_modifier = tf.nn.l2_loss(y2 - y2_)
            log_dict['modifier/loss'] = loss_modifier*loss_normalizer
            if pargs.mve:
                loss_modifier_mve = 0.5*tf.reduce_sum(((y2 - y2_)**2)/(s**2 + 1e-4)) + 0.5*tf.reduce_sum(tf.log(s**2 + 1e-4)) # mean variance estimate
                log_dict['modifier_mve/loss'] = loss_modifier_mve*loss_normalizer

        loss = loss_detector + loss_modifier
        log_dict['sum_loss'] = log_dict['detector/loss'] + log_dict['modifier/loss']
        if pargs.mve:
            loss_mve = loss_detector + loss_modifier_mve
            log_dict['sum_loss_mve'] = log_dict['detector/loss'] + log_dict['modifier_mve/loss']

        if pargs.decay>0.0:
            w_detector     = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES, "detector/")
            w_modifier     = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES, "modifier/")
            decay_detector = tf.add_n([tf.nn.l2_loss(v) for v in w_detector])*pargs.decay
            decay_modifier = tf.add_n([tf.nn.l2_loss(v) for v in w_modifier])*pargs.decay

            loss += decay_detector + decay_modifier

            log_dict['detector/decay'] = decay_detector
            log_dict['modifier/decay'] = decay_modifier
            log_dict['sum_loss'] += decay_detector + decay_modifier

            if pargs.mve:
                w_modifier_var = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES, "modifier_var/")
                decay_modifier_var = tf.add_n([tf.nn.l2_loss(v) for v in w_modifier_var])*pargs.decay
                loss_mve += decay_modifier_var
                log_dict['modifier_var/decay'] = decay_modifier_var
                log_dict['sum_loss_mve'] += decay_modifier_var

    for i in log_dict:
        tf.summary.scalar(name=i, tensor=log_dict[i])

with tf.name_scope('train'):
    train_step = tf.train.AdamOptimizer(1e-4).minimize(loss)
    if pargs.mve: train_step_mve = tf.train.AdamOptimizer(1e-4).minimize(loss_mve)

increment_global_step = tf.assign_add(global_step, 1, name='increment_global_step')

def model_summary():
    model_vars = tf.trainable_variables()
    slim.model_analyzer.analyze_vars(model_vars, print_info=True)

model_summary()

################################################################################
# load / train

exit()

saver = tf.train.Saver()
if pargs.load:
    saver.restore(sess, tf.train.latest_checkpoint(pargs.output))
    last_step = sess.run(global_step)
    logging.info('Trained model is loaded ({})'.format(pargs.output))

else:
    tf.global_variables_initializer().run()

merged = tf.summary.merge_all()
train_writer = tf.summary.FileWriter(pargs.output + '/summary/train')
test_writer  = tf.summary.FileWriter(pargs.output + '/summary/test')
if not pargs.load: train_writer.add_graph(sess.graph)

batch_test = data_sets.test.get_data()
feed_data_test = { x: batch_test['inputs'], y_: batch_test['labels'], y2_: batch_test['modvel']/scale['modvel'] }
if pargs.ddrop>0.0: feed_data_test.update({keep_prob : 1.0})
if pargs.mdrop>0.0: feed_data_test.update({keep_prob2: 1.0})

losses_key = sorted([k for k in log_dict if 'loss' in k])
losses_fetch = [log_dict[k] for k in losses_key]
for i in range(last_step if pargs.load else 0, pargs.steps):
    batch = data_sets.train.next_batch(pargs.batch)

    fetches = [merged, train_step, increment_global_step]
    feed_data = { x: batch['inputs'], y_: batch['labels'], y2_: batch['modvel']/scale['modvel'] }

    if pargs.mve and i>int(pargs.steps/2):
        fetches = [merged, train_step_mve, increment_global_step]
        if pargs.mdrop>0.0: y2_appx = sess.run(y2, feed_dict={ x: batch['inputs'], keep_prob2: 1.0 })
        else:               y2_appx = sess.run(y2, feed_dict={ x: batch['inputs'] })
        feed_data = { x: batch['inputs'], y_: batch['labels'], y2_: batch['modvel']/scale['modvel'], y2: y2_appx }

    if pargs.ddrop>0.0: feed_data.update({keep_prob : 1.0-pargs.ddrop})
    if pargs.mdrop>0.0: feed_data.update({keep_prob2: 1.0-pargs.mdrop})

    params_sess_run = dict(fetches=fetches, feed_dict=feed_data)
    if (i % pargs.lstep*10 == 0):
        run_metadata = tf.RunMetadata()
        params_sess_run.update(options=tf.RunOptions(trace_level=tf.RunOptions.FULL_TRACE), run_metadata=run_metadata)

    summary = sess.run(**params_sess_run)[0]

    if (i % pargs.lstep*10 == 0):
        train_writer.add_run_metadata(run_metadata, 'step{:03d}'.format(i))

    if (i % pargs.lstep == 0):           # evaluate losses with the test data set
        train_writer.add_summary(summary, i)

    if (i % pargs.lstep == 0):           # evaluate losses with the test data set
        summary, acc, losses = sess.run([merged, accuracy, losses_fetch], feed_dict=feed_data_test)
        test_writer.add_summary(summary, i)
        print('At step {}: accuracy={:.5f}, {}'.format(i, acc, ', '.join('{}={:.5f}'.format(*t) for t in zip(losses_key, losses))))

# the last test
summary, acc_summary = sess.run([merged, list(log_dict.values())], feed_dict=feed_data_test)
test_writer.add_summary(summary, i)

train_writer.close()
test_writer.close()

logging.info(dict(zip(log_dict.keys(), acc_summary)))

# save the trained model
model_file = saver.save(sess, '{}/model.ckpt'.format(pargs.output))
logging.info('Trained model saved to {}'.format(model_file))

sess.close()
