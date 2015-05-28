#!/usr/bin/env python
import sys, os, random
from PyQt4.QtCore import *
from PyQt4.QtGui import *

import matplotlib
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg as NavigationToolbar
from matplotlib.figure import Figure
from matplotlib.patches import Circle, Wedge
from matplotlib.collections import PatchCollection, LineCollection

from threading import Thread
import zmq

import numpy as np
import pandas as pd

FOREST = pd.read_csv('../virtual_forest/forests/forest.csv')

class AppForm(QMainWindow):
    def __init__(self, parent=None):
        QMainWindow.__init__(self, parent)
        self.setWindowTitle('MothVR Demo')

        self.create_menu()
        self.create_main_frame()


        self.pos_x = [0]
        self.pos_y = [0]
        self.theta = [0]

        self.traj = self.axes.plot(self.pos_x, self.pos_y, '-k')[0]
        self.plot_trees()

        z_thread = Thread(target=self.zmq_thread)
        z_thread.start()

        self.timer = QTimer()
        self.timer.timeout.connect(self.on_draw)
        self.timer.start(50)

        self.window_size=10

        self.running = True

        self.on_draw()

    def zmq_thread(self):
        context = zmq.Context.instance()

        expt = context.socket(zmq.SUB)
        expt.connect('tcp://localhost:5555')
        expt.setsockopt(zmq.SUBSCRIBE, "")

        pose = context.socket(zmq.SUB)
        pose.connect('tcp://localhost:5561')
        pose.setsockopt(zmq.SUBSCRIBE, "")

        poller = zmq.Poller()
        poller.register(expt, zmq.POLLIN)
        poller.register(pose, zmq.POLLIN)

        while self.running:
            try:
                socks=dict(poller.poll())
            except KeyboardInterrupt:
                break

            if expt in socks:
                msg = expt.recv()
                if msg[:5]=="START":
                    self.pos_x = [0]
                    self.pos_y = [0]
                    self.theta = [0]
                if msg=="END":
                    self.running=False
                    self.close()


            if pose in socks:
                msg=pose.recv()

                if len(msg.split())==8:
                    data = msg.split()
                    self.pos_x.append(float(data[1]))
                    self.pos_y.append(float(data[3]))
                    self.theta.append(float(data[5]))

    def plot_trees(self, circles=True):
        '''
        Given a matplotlib axes object, add circle patches where the trees should
        go.

        '''

        tree_list = []

        for i, tree in FOREST.iterrows():
            t = Circle((tree['x'], tree['y']), tree['r'])
            tree_list.append(t)

        p = PatchCollection(tree_list, edgecolor='none', facecolor='#8DA893')
        self.axes.add_collection(p)

        if circles:
            r = np.sqrt(FOREST['x']**2 + FOREST['y']**2)
            mean_r = np.mean(r)
            std_r = np.std(r)
            max_r = np.max(r)
            self.axes.add_patch(Circle((0,0), mean_r, facecolor='none',
                edgecolor='0.5'))
            self.axes.add_patch(Circle((0,0), mean_r - std_r, facecolor='none',
                edgecolor='0.75', linestyle='dashed'))
            self.axes.add_patch(Circle((0,0), mean_r + std_r, facecolor='none',
                edgecolor='0.75', linestyle='dashed'))
            self.axes.add_patch(Circle((0,0), max_r, facecolor='none',
                edgecolor='#990000'))

    def on_draw(self):
        """ Redraws the figure
        """

        self.traj.set_xdata(self.pos_x)
        self.traj.set_ydata(self.pos_y)
        self.axes.axis('equal')
        self.axes.set_xbound(self.pos_x[-1] - self.window_size, self.pos_x[-1]+self.window_size)
        self.axes.set_ybound(self.pos_y[-1] - self.window_size, self.pos_y[-1]+self.window_size)

        self.canvas.draw()

    def create_main_frame(self):
        self.main_frame = QWidget()

        # Create the mpl Figure and FigCanvas objects.
        # 5x4 inches, 100 dots-per-inch
        #
        self.dpi = 100
        self.fig = Figure((5.0, 4.0), dpi=self.dpi)
        self.canvas = FigureCanvas(self.fig)
        self.canvas.setParent(self.main_frame)

        # Since we have only one plot, we can use add_axes
        # instead of add_subplot, but then the subplot
        # configuration tool in the navigation toolbar wouldn't
        # work.
        #
        self.axes = self.fig.add_subplot(111)
        vbox = QVBoxLayout()

        vbox.addWidget(self.canvas)

        self.main_frame.setLayout(vbox)
        self.setCentralWidget(self.main_frame)

    def create_menu(self):
        self.file_menu = self.menuBar().addMenu("&File")

        quit_action = self.create_action("&Quit", slot=self.close,
            shortcut="Ctrl+Q", tip="Close the application")

        self.file_menu.addAction(quit_action)

    def add_actions(self, target, actions):
        for action in actions:
            if action is None:
                target.addSeparator()
            else:
                target.addAction(action)

    def create_action(  self, text, slot=None, shortcut=None,
                        icon=None, tip=None, checkable=False,
                        signal="triggered()"):
        action = QAction(text, self)
        if icon is not None:
            action.setIcon(QIcon(":/%s.png" % icon))
        if shortcut is not None:
            action.setShortcut(shortcut)
        if tip is not None:
            action.setToolTip(tip)
            action.setStatusTip(tip)
        if slot is not None:
            self.connect(action, SIGNAL(signal), slot)
        if checkable:
            action.setCheckable(True)
        return action

    def closeEvent(self, event):
        self.running = False
        event.accept()


def main():
    app = QApplication(sys.argv)
    form = AppForm()
    form.show()
    app.exec_()


if __name__ == "__main__":
    main()
