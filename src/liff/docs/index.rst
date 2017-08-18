.. _liff:

LIFF
====

LIFF is the Likelihood Fitting Framework bundled with AERIE. In its current
formulation the framework is used to forward-fold parametric spectral and
spatial models through the HAWC detector response to compare a binned set of
observed counts :math:`N_\text{obs}` to an expected set of counts given a set
of model parameters :math:`\theta`. The likelihood is computed as

.. math::
  \ln{\mathcal{L}(\theta;N_\text{obs})} =
  \sum_{i=0}^{N_\text{bins}}\ln{\left(N_{\text{obs},i}|\theta\right)}.

The optimum set of model parameters :math:`\theta_0` is obtained by minimizing
:math:`-\ln{\mathcal{L}}`. To compare hypotheses a likelihood ratio test is
computed, giving a test statistic

.. math::
  \text{TS} = 2\ln{\frac{\mathcal{L}(\mathcal{H}_1;N_\text{obs})}{\mathcal{L}(\mathcal{H}_0;N_\text{obs})}},

where :math:`\mathcal{H}_0` represents a null hypothesis and
:math:`\mathcal{H}_1` represents an alternative hypothesis.

For more details, see
`this conference proceeding <https://arxiv.org/abs/1508.07479>`_.
