#!/usr/bin/env python

from hawc import hawcnest, data_structures
from hawc.hawcnest import HAWCUnits as HAWCUnits
from hawc.data_structures import *
from hawc import rng_service as rng_service
from HAWCNest import HAWCNest

from optparse import OptionParser

import pylab as P
import numpy as N
from matplotlib import gridspec as gridspec
from matplotlib.font_manager import FontProperties



#############################################################################
class PrettyGraph:
  """Makes a pretty graph
  """

  def __init__(self,majorticklength = 12,minorticklength = 6,toptitleoffset = 0.02,linewidth = 1.5,labelfontsize = 15,titlefontsize = 20,backgroundcolor = "white",xtitle = "X",ytitle = "Y",usetitle = False,title = "Title",limits = [0.1,0.97,0.12,0.95],ylabeladjust = 0.93,xlabeladjust = 0.9):
    self.linewidth = linewidth
    self.labelfontsize = labelfontsize
    self.backgroundcolor = backgroundcolor
    self.xtitle = xtitle
    self.ytitle = ytitle
    self.usetitle = usetitle
    self.title = title
    self.limits = limits
    self.majorticklength = majorticklength
    self.minorticklength = minorticklength
    self.toptitleoffset = toptitleoffset
    self.titlefontsize = titlefontsize
    self.xlabeladjust = xlabeladjust
    self.ylabeladjust = ylabeladjust

    toplimit = limits[3]
    if(self.usetitle):
      toplimit = toplimit-self.toptitleoffset
    self.fig = P.figure()
    self.gs1 = gridspec.GridSpec(1,1)
    self.gs1.update(left = self.limits[0],right = self.limits[1],bottom = self.limits[2],top = toplimit,wspace = 0.05)
    self.ax = self.fig.add_subplot(self.gs1[0,0])

  def make_graph(self):
    rect = self.fig.patch
    rect.set_facecolor(self.backgroundcolor)
    self.ax.set_xlabel(self.xtitle,fontsize = self.labelfontsize)
    self.ax.set_ylabel(self.ytitle,fontsize = self.labelfontsize)
    self.ax.get_xaxis().get_label().set_x(self.xlabeladjust)
    self.ax.get_yaxis().get_label().set_y(self.ylabeladjust)
    P.setp(self.ax.get_xticklabels(),fontsize = self.labelfontsize)
    P.setp(self.ax.get_yticklabels(),fontsize = self.labelfontsize)
    P.tick_params(which = "both",width = self.linewidth)
    P.tick_params(which = "major",length = self.majorticklength)
    P.tick_params(which = "minor",length = self.minorticklength)
    for i in self.ax.spines.itervalues():
      i.set_linewidth(self.linewidth)
    if(self.usetitle):
      self.fig.suptitle(self.title,fontsize = self.titlefontsize)
    return self.fig,self.ax

  def save(self,filename):
    self.fig.savefig(filename,facecolor = self.backgroundcolor)



#############################################################################
class HistogramConverter:
  """Converts histograms to reasonable plots
  """

  def __init__(self,data,binstyle = "linear",lowedge = 0.0,
               highedge = 1.0,bincount = 10,weights = None,
               opts = "ro",label = None,norm = False):
    self.binstyle = binstyle
    self.lowedge = lowedge
    self.highedge = highedge
    self.bincount = bincount
    self.data = data
    self.weights = N.ones(len(self.data))
    if(weights != None):
      self.weights = weights
    self.opts = opts
    self.label = label
    factor = (1.0/len(self.data) if norm else 1.0)
    self.weights = [factor*i for i in self.weights]

  def make_histogram(self):
    edges = []
    # Set up evenly spaced bins and determine their centers
    if(self.binstyle == "log"):
      edges = 10.0**N.linspace(P.log10(self.lowedge),
                               P.log10(self.highedge),self.bincount+1)
      self.centers = N.sqrt(edges[1:]*edges[:-1])
    else:
      edges = N.linspace(self.lowedge,self.highedge,self.bincount+1)
      self.centers = 0.5*(edges[1:]+edges[:-1])
    self.widths = (edges[1:]-edges[:-1])
    # Make the histogram and error histogram
    self.ndata,temp = N.histogram(self.data,bins = edges,
                              weights = self.weights)
    sqweights = [i*i for i in self.weights]
    self.nweights,temp = N.histogram(self.data,bins = edges,
                                 weights = sqweights)
    self.errors = N.sqrt(self.nweights)

  def make_plot(self,ax):
    self.make_histogram()
    if(self.label != None):
      return ax.plot(self.centers,self.ndata[0:]/self.widths[0:],
                     self.opts,label = self.label)
    return ax.plot(self.centers,self.ndata[0:]/self.widths[0:],
                   self.opts)



#############################################################################
def getRelIntErr(pl,breaks,params,limits):
  value = evalPowerLawIntegral(breaks,params,limits)
  val = pl.integrate(limits[0],limits[1])
  return (val-value)/value



#############################################################################
def evalPowerLawIntegral(breaks,params,limits):
  if(breaks == 2):
    p1s = [params[0],params[1],params[2],params[3],params[4]]
    p2s = [params[0],params[1],
           params[2]*((params[5]/params[4])**(params[3]-params[6])),
           params[6],params[4]]
    p3s = [params[0],params[1],
           params[2]*((params[5]/params[4])**(params[3]-params[6])) * \
           ((params[7]/params[4])**(params[6]-params[8])),
           params[8],params[4]]
    l1s = [limits[0],params[5]]
    l2s = [params[5],params[7]]
    l3s = [params[7],limits[1]]
    l21s = [params[5],limits[1]]
    l23s = [limits[0],params[7]]
    if(limits[0] < params[5] and limits[1] < params[5]):
      return evalPowerLawIntegral(0,p1s,limits)
    if(params[5] < limits[0] and limits[1] < params[7]):
      return evalPowerLawIntegral(0,p2s,limits)
    if(params[7] < limits[1] and params[7] < limits[0]):
      return evalPowerLawIntegral(0,p3s,limits)
    if(limits[0] < params[5] and limits[1] < params[7]
       and params[5] < limits[1]):
      return evalPowerLawIntegral(0,p1s,l1s) + \
             evalPowerLawIntegral(0,p2s,l21s)
    if(limits[0] < params[7] and params[5] < limits[0]
       and params[7] < limits[1]):
      return evalPowerLawIntegral(0,p2s,l23s) + \
             evalPowerLawIntegral(0,p3s,l3s)
    return evalPowerLawIntegral(0,p1s,l1s) + \
           evalPowerLawIntegral(0,p2s,l2s) + \
           evalPowerLawIntegral(0,p3s,l3s)
  elif(breaks == 1):
    p1s = [params[0],params[1],params[2],params[3],params[4]]
    p2s = [params[0],params[1],
           params[2]*((params[5]/params[4])**(params[3]-params[6])),
           params[6],params[4]]
    l1s = [limits[0],params[5]]
    l2s = [params[5],limits[1]]
    if(limits[0] < params[5] and limits[1] < params[5]):
      return evalPowerLawIntegral(0,p1s,limits)
    if(limits[0] >= params[5] and limits[1] >= params[5]):
      return evalPowerLawIntegral(0,p2s,limits)
    return evalPowerLawIntegral(0,p1s,l1s) + \
           evalPowerLawIntegral(0,p2s,l2s)
  if(params[3] == -1.0):
    return params[2]*params[4]*N.log(limits[1]/limits[0])
  if(params[1] == HAWCUnits.infinity):
    return params[2]/(params[3]+1.0)/(params[4]**(params[3]))* \
           (-limits[0]**(params[3]+1.0))
  return params[2]/(params[3]+1.0)/(params[4]**(params[3]))* \
         (limits[1]**(params[3]+1.0)-limits[0]**(params[3]+1.0))
  


#############################################################################



def runIntTests():
  limits = [0.2,12.0]
  inflimits = [0.2,HAWCUnits.infinity]
  params = [0.3,3.0,0.6,-2.4,0.77]
  ptest = PowerLaw(params[0],params[1],params[2],params[4],params[3])
  print "===================="
  print "Relative errors"
  print "===================="
  print "PowerLaw"
  print "--------------------"
  print "Integral 1:",getRelIntErr(ptest,0,params,limits)
  print "Integral 2:",getRelIntErr(ptest,0,params,inflimits)
  params[3] = -1.0
  ptest = PowerLaw(params[0],params[1],params[2],params[4],params[3])
  print "Integral 3:",getRelIntErr(ptest,0,params,limits)
  print "===================="
  print "BrokenPowerLaw"
  print "--------------------"
  params.append(0.5)
  params.append(-3.1)
  ptest = BrokenPowerLaw(params[0],params[1],params[2],params[4],
                         params[3],params[5],params[6])
  limits = [0.2,12.0]
  print "Integral 1:",getRelIntErr(ptest,1,params,limits)
  limits = [0.33,0.354]
  print "Integral 2:",getRelIntErr(ptest,1,params,limits)
  limits = [0.55,6.8]
  print "Integral 3:",getRelIntErr(ptest,1,params,limits)
  inflimits[0] = 0.2
  print "Integral 4:",getRelIntErr(ptest,1,params,inflimits)
  inflimits[0] = 6.7
  print "Integral 5:",getRelIntErr(ptest,1,params,inflimits)
  params[3] = -1.0
  ptest = BrokenPowerLaw(params[0],params[1],params[2],params[4],
                         params[3],params[5],params[6])
  limits = [0.2,12.0]
  print "Integral 6:",getRelIntErr(ptest,1,params,limits)
  limits = [0.33,0.35]
  print "Integral 7:",getRelIntErr(ptest,1,params,limits)
  limits = [0.55,6.7]
  print "Integral 8:",getRelIntErr(ptest,1,params,limits)
  params[3] = -2.4
  params[6] = -1.0
  ptest = BrokenPowerLaw(params[0],params[1],params[2],params[4],
                         params[3],params[5],params[6])
  limits = [0.2,12.0]
  print "Integral 9:",getRelIntErr(ptest,1,params,limits)
  limits = [0.33,0.35]
  print "Integral 10:",getRelIntErr(ptest,1,params,limits)
  limits = [0.55,6.7]
  print "Integral 11:",getRelIntErr(ptest,1,params,limits)
  print "===================="
  print "DoubleBrokenPowerLaw"
  print "--------------------"
  params = [0.3,3.0,0.6,-2.4,0.77,0.5,-3.1,1.2,-1.04]
  ptest = DoubleBrokenPowerLaw(params[0],params[1],params[2],params[4],
                               params[3],params[5],params[6],params[7],
                               params[8])
  limits = [0.2,12.0]
  print "Integral 1:",getRelIntErr(ptest,2,params,limits)
  limits = [0.2,0.67]
  print "Integral 2:",getRelIntErr(ptest,2,params,limits)
  limits = [0.2,0.22]
  print "Integral 3:",getRelIntErr(ptest,2,params,limits)
  limits = [0.58,12.0]
  print "Integral 4:",getRelIntErr(ptest,2,params,limits)
  limits = [0.58,1.11]
  print "Integral 5:",getRelIntErr(ptest,2,params,limits)
  limits = [1.8,12.0]
  print "Integral 6:",getRelIntErr(ptest,2,params,limits)
  inflimits[0] = 0.2
  print "Integral 7:",getRelIntErr(ptest,2,params,inflimits)
  inflimits[0] = 0.8
  print "Integral 8:",getRelIntErr(ptest,2,params,inflimits)
  inflimits[0] = 35.0
  print "Integral 9:",getRelIntErr(ptest,2,params,inflimits)
  params = [0.3,3.0,0.6,-1.0,0.77,0.5,-3.1,1.2,-1.04]
  ptest = DoubleBrokenPowerLaw(params[0],params[1],params[2],params[4],
                               params[3],params[5],params[6],params[7],
                               params[8])
  limits = [0.2,12.0]
  print "Integral 10:",getRelIntErr(ptest,2,params,limits)
  limits = [0.2,0.67]
  print "Integral 11:",getRelIntErr(ptest,2,params,limits)
  limits = [0.2,0.22]
  print "Integral 12:",getRelIntErr(ptest,2,params,limits)
  limits = [0.58,12.0]
  print "Integral 13:",getRelIntErr(ptest,2,params,limits)
  limits = [0.58,1.11]
  print "Integral 14:",getRelIntErr(ptest,2,params,limits)
  limits = [1.8,12.0]
  print "Integral 15:",getRelIntErr(ptest,2,params,limits)
  inflimits[0] = 0.2
  print "Integral 16:",getRelIntErr(ptest,2,params,inflimits)
  inflimits[0] = 0.8
  print "Integral 17:",getRelIntErr(ptest,2,params,inflimits)
  inflimits[0] = 35.0
  print "Integral 18:",getRelIntErr(ptest,2,params,inflimits)
  params = [0.3,3.0,0.6,-1.8,0.77,0.5,-1.0,1.2,-1.04]
  ptest = DoubleBrokenPowerLaw(params[0],params[1],params[2],params[4],
                               params[3],params[5],params[6],params[7],
                               params[8])
  limits = [0.2,12.0]
  print "Integral 19:",getRelIntErr(ptest,2,params,limits)
  limits = [0.2,0.67]
  print "Integral 20:",getRelIntErr(ptest,2,params,limits)
  limits = [0.2,0.22]
  print "Integral 21:",getRelIntErr(ptest,2,params,limits)
  limits = [0.58,12.0]
  print "Integral 22:",getRelIntErr(ptest,2,params,limits)
  limits = [0.58,1.11]
  print "Integral 23:",getRelIntErr(ptest,2,params,limits)
  limits = [1.8,12.0]
  print "Integral 24:",getRelIntErr(ptest,2,params,limits)
  inflimits[0] = 0.2
  print "Integral 25:",getRelIntErr(ptest,2,params,inflimits)
  inflimits[0] = 0.8
  print "Integral 26:",getRelIntErr(ptest,2,params,inflimits)
  inflimits[0] = 35.0
  print "Integral 27:",getRelIntErr(ptest,2,params,inflimits)
  params = [0.3,3.0,0.6,-1.0,0.77,0.5,-1.0,1.2,-1.0]
  ptest = DoubleBrokenPowerLaw(params[0],params[1],params[2],params[4],
                               params[3],params[5],params[6],params[7],
                               params[8])
  limits = [0.2,12.0]
  print "Integral 28:",getRelIntErr(ptest,2,params,limits)
  limits = [0.2,0.67]
  print "Integral 29:",getRelIntErr(ptest,2,params,limits)
  limits = [0.2,0.22]
  print "Integral 30:",getRelIntErr(ptest,2,params,limits)
  limits = [0.58,12.0]
  print "Integral 31:",getRelIntErr(ptest,2,params,limits)
  limits = [0.58,1.11]
  print "Integral 32:",getRelIntErr(ptest,2,params,limits)
  limits = [1.8,12.0]
  print "Integral 33:",getRelIntErr(ptest,2,params,limits)
  print "===================="



#############################################################################
def main():
  usage = "%prog [options] [N = size of random data set]"
  parser = OptionParser(usage)
  parser.add_option("-s","--seed",dest = "seed",type = int,
                 default = 54112,
                 help = "Seed for random number generator")
  parser.add_option("","--notest",dest = "test",action = "store_false",
                 default = True,help = "Skip integral tests")

  (opts,args) = parser.parse_args()

  if(len(args) != 1):
    parser.error("Wrong number of arguments!")

  nest = HAWCNest()
  nest.Service("StdRNGService","rng",seed = opts.seed)

  nest.Configure()

  rng = rng_service.GetService("rng")

  x0,x1,idx1,idx2 = [1.0,100.0,-2.0,-0.0]
  norm1 = 1.0/PowerLaw(x0,x1,1.0,x0,idx1).integrate(x0,x1)
  norm2 = 1.0/PowerLaw(x0,x1,1.0,x0,idx2).integrate(x0,x1)
  norm3 = 1.0/BrokenPowerLaw(x0,x1,1.0,x0,-1.0,10.0,-2.0).integrate(x0,x1)
  norm4 = 1.0/BrokenPowerLaw(x0,x1,1.0,x0,-2.0,10.0,-1.0).integrate(x0,x1)
  norm5 = 1.0/DoubleBrokenPowerLaw(x0,x1,1.0,x0,-1.4,4.5,0.4,34.2,-1.0). \
          integrate(x0,x1)
  norm6 = 1.0/CutoffPowerLaw(x0,x1,1.0,x0,-1.0,10.0).integrate(x0,x1)
  norm7 = 1.0/PowerLaw(x0,x1,1.0,x0,-4.7).integrate(x0,x1)
  p1 = PowerLaw(x0,x1,norm1,x0,idx1)
  p2 = PowerLaw(x0,x1,norm2,x0,idx2)
  p3 = BrokenPowerLaw(x0,x1,10.0*norm3,x0,-1.0,10.0,-2.0)
  p4 = BrokenPowerLaw(x0,x1,0.1*norm4,x0,-2.0,10.0,-1.0)
  p5 = DoubleBrokenPowerLaw(x0,x1,norm6,x0,-1.4,4.5,0.4,34.2,-1.0)
  p6 = CutoffPowerLaw(x0,x1,norm5,x0,-1.0,10.0)
  p7 = PowerLaw(x0,x1,norm7,x0,-4.7)
  p8 = LogParabola(x0,x1,1.0,x0,-0.3,-0.7)

  # Test the various power laws
  if(opts.test):
    runIntTests()

  print "Sampling",args[0],("point" if int(args[0]) == 1 else "points")
  p1sample = []
  p2weight = []
  p3weight = []
  p4weight = []
  p5sample = []
  p6weight = []
  p7weight = []
  p8weight = []
  p2keeps = []
  for i in range(0,int(args[0])):
    val = p1.invert_integral(rng.Uniform())
    p1sample.append(val)
    p2weight.append(p2.reweight(p1,val))
    p3weight.append(p3.reweight(p1,val))
    p4weight.append(p4.reweight(p1,val))
    val2 = p5.invert_integral(rng.Uniform())
    p5sample.append(val2)
    p6weight.append(p6.reweight(p5,val2))
    p7weight.append(p7.reweight(p5,val2))
    p8weight.append(p8.reweight(p5,val2))
    if(rng.Uniform() < p2.prob_to_keep(p1,val)):
      p2keeps.append(val)

  # Get the power law points
  xplot = 10.0**N.linspace(P.log10(x0),P.log10(x1),1000)
  y1plot = []
  y2plot = []
  y3plot = []
  y4plot = []
  y5plot = []
  y6plot = []
  y7plot = []
  y8plot = []
  for i in range(0,len(xplot)):
    y1plot.append(p1.evaluate(xplot[i])/p1.integrate(x0,x1))
    y2plot.append(p2.evaluate(xplot[i]))
    y3plot.append(p3.evaluate(xplot[i]))
    y4plot.append(p4.evaluate(xplot[i]))
    y5plot.append(p5.evaluate(xplot[i])/p5.integrate(x0,x1))
    y6plot.append(p6.evaluate(xplot[i]))
    y7plot.append(p7.evaluate(xplot[i]))
    y8plot.append(p8.evaluate(xplot[i]))

  # Make a log-log histogram of the random numbers
  gph1 = PrettyGraph(usetitle = True,xtitle = "X",
                  title = "Reweighting power law with index "+str(idx1),
                  ytitle = "Counts",ylabeladjust = 0.92,
                  xlabeladjust = 0.96,limits = [0.11,0.97,0.12,0.95])
  hc1 = HistogramConverter(data = p1sample,lowedge = x0,highedge = x1,
                           bincount = 21,binstyle = "log",opts = "ro",
                           label = "Index -2",norm = True)
  hc2 = HistogramConverter(data = p1sample,lowedge = x0,highedge = x1,
                           bincount = 21,binstyle = "log",opts = "bo",
                           label = "Index -1",norm = True,
                           weights = p2weight)
  hc3 = HistogramConverter(data = p1sample,lowedge = x0,highedge = x1,
                           bincount = 21,binstyle = "log",opts = "go",
                           label = "Broken -2 to -1",norm = True,
                           weights = p3weight)
  hc4 = HistogramConverter(data = p1sample,lowedge = x0,highedge = x1,
                           bincount = 21,binstyle = "log",opts = "yo",
                           label = "Broken -1 to -2",norm = True,
                           weights = p4weight)
  fig,ax = gph1.make_graph()
  ax.set_xscale("log")
  ax.set_yscale("log")
  ax.set_autoscaley_on(False)
  ax.set_ylim([1.0e-4,10.0])
  hc1.make_plot(ax)
  hc2.make_plot(ax)
  hc3.make_plot(ax)
  hc4.make_plot(ax)
  ax.plot(xplot,y1plot,"r--")
  ax.plot(xplot,y2plot,"b--")
  ax.plot(xplot,y3plot,"g--")
  ax.plot(xplot,y4plot,"y--")
  handles,labels = ax.get_legend_handles_labels()
  leg = ax.legend(handles,labels,numpoints = 1,bbox_to_anchor = [0.97,0.95])
  leg.get_frame().set_linewidth(0)

  # Now make it for the more complicated cases
  gph2 = PrettyGraph(usetitle = True,xtitle = "X",
                  title = "Reweighting double broken power law",
                  ytitle = "Counts",ylabeladjust = 0.92,
                  xlabeladjust = 0.96,limits = [0.11,0.97,0.12,0.95])
  hc5 = HistogramConverter(data = p5sample,lowedge = x0,highedge = x1,
                           bincount = 21,binstyle = "log",opts = "ro",
                           label = "Double break sample",norm = True)
  hc6 = HistogramConverter(data = p5sample,lowedge = x0,highedge = x1,
                           bincount = 21,binstyle = "log",opts = "bo",
                           label = "Cutoff",norm = True,
                           weights = p6weight)
  hc7 = HistogramConverter(data = p5sample,lowedge = x0,highedge = x1,
                           bincount = 21,binstyle = "log",opts = "go",
                           label = "Power law",norm = True,
                           weights = p7weight)
  hc8 = HistogramConverter(data = p5sample,lowedge = x0,highedge = x1,
                           bincount = 21,binstyle = "log",opts = "yo",
                           label = "Log parabola",norm = True,
                           weights = p8weight)
  fig2,ax2 = gph2.make_graph()
  ax2.set_xscale("log")
  ax2.set_yscale("log")
  ax2.set_autoscaley_on(False)
  ax2.set_ylim([1.0e-9,10.0])
  hc5.make_plot(ax2)
  hc6.make_plot(ax2)
  hc7.make_plot(ax2)
  hc8.make_plot(ax2)
  ax2.plot(xplot,y5plot,"r--")
  ax2.plot(xplot,y6plot,"b--")
  ax2.plot(xplot,y7plot,"g--")
  ax2.plot(xplot,y8plot,"y--")
  handles2,labels2 = ax2.get_legend_handles_labels()
  leg2 = ax2.legend(handles2,labels2,numpoints = 1,bbox_to_anchor = [0.45,0.32])
  leg2.get_frame().set_linewidth(0)

  gph3 = PrettyGraph(usetitle = True,xtitle = "X",
                     title = "Resampling power law",
                     ytitle = "Counts",ylabeladjust = 0.92,
                     xlabeladjust = 0.96,limits = [0.11,0.97,0.12,0.95])
  hc9 = HistogramConverter(data = p2keeps,lowedge = x0,highedge = x1,
                           bincount = 21,binstyle = "log",opts = "bo",
                           label = "Resampled",norm = True)
  fig3,ax3 = gph3.make_graph()
  ax3.set_xscale("log")
  ax3.set_yscale("log")
  ax3.set_autoscaley_on(False)
  ax3.set_ylim([1.0e-4,1.0])
  hc1.make_plot(ax3)
  hc9.make_plot(ax3)
  ax3.plot(xplot,y1plot,"r--")
  ax3.plot(xplot,y2plot,"b--")
  handles3,labels3 = ax3.get_legend_handles_labels()
  leg3 = ax3.legend(handles3,labels3,numpoints = 1,bbox_to_anchor = [0.97,0.95])
  leg3.get_frame().set_linewidth(0)

  P.show()

if __name__ == "__main__":
    main()

