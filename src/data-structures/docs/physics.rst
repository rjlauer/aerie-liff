.. _data_structures_physics:

Physics Classes
===============

.. contents::
   :local:
   :backlinks: top

Particle Types
--------------

The physics classes in **data structures** define `particle type codes
<../../doxygen/html/namespaceParticle.html>`_ for use in simulation.  These
codes correspond to the same IDs used in the `CORSIKA
<http://www-ik.fzk.de/corsika/>`_ air shower library, making comparison between
the shower files and the detector reconstruction simple.

Functions are provided to convert particle IDs to strings and back, and to
calculate the mass and charge of a given particle.  The particle IDs defined in
the library, a subset of the IDs available in CORSIKA, are given below:

====================== ======================= ====
Particle               Enum                    ID
====================== ======================= ====
unknown                :code:`unknown`            0
:math:`\gamma`         :code:`gamma`              1
:math:`e^{+}`          :code:`EPlus`              2
:math:`e^{-}`          :code:`EMinus`             3
:math:`\mu^{+}`        :code:`MuPlus`             5 
:math:`\mu^{-}`        :code:`MuMinus`            6
:math:`\pi^{0}`        :code:`Pi0`                7
:math:`\pi^{+}`        :code:`PiPlus`             8
:math:`\pi^{-}`        :code:`PiMinus`            9
:math:`K_{L}^{0}`      :code:`K0_Long`           10 
:math:`K^{+}`          :code:`KPlus`             11
:math:`K^{-}`          :code:`KMinus`            12
:math:`n`              :code:`Neutron`           13
:math:`p`              :code:`PPlus`             14
:math:`\bar{p}`        :code:`PMinus`            15
:math:`K_{S}^{0}`      :code:`K0_Short`          16
:math:`\bar{n}`        :code:`NeutronBar`        25
:math:`\nu_e`          :code:`NuE`               66
:math:`\bar{\nu}_e`    :code:`NuE`               67
:math:`\nu_\mu`        :code:`NuMu`              68
:math:`\bar{\nu}_\mu`  :code:`NuMuBar`           69
:math:`\tau^{+}`       :code:`TauPlus`          131 
:math:`\tau^{-}`       :code:`TauMinus`         132
:math:`\nu_\tau`       :code:`NuTau`            133
:math:`\bar{\nu}_\tau` :code:`NuTauBar`         134
:sup:`4`\ He           :code:`He4Nucleus`       402
:sup:`7`\ Li           :code:`Li7Nucleus`       703
:sup:`9`\ Be           :code:`Be9Nucleus`       904
:sup:`11`\ B           :code:`B11Nucleus`      1105
:sup:`12`\ C           :code:`C12Nucleus`      1206
:sup:`14`\ N           :code:`N14Nucleus`      1407
:sup:`16`\ O           :code:`O16Nucleus`      1608
:sup:`19`\ F           :code:`F19Nucleus`      1909
:sup:`20`\ Ne          :code:`Ne20Nucleus`     2010
:sup:`23`\ Na          :code:`Na23Nucleus`     2311
:sup:`24`\ Mg          :code:`Mg24Nucleus`     2412
:sup:`27`\ Al          :code:`Al27Nucleus`     2713
:sup:`28`\ Si          :code:`Si28Nucleus`     2814
:sup:`31`\ P           :code:`P31Nucleus`      3115
:sup:`32`\ S           :code:`S32Nucleus`      3216
:sup:`35`\ Cl          :code:`Cl35Nucleus`     3517
:sup:`40`\ Ar          :code:`Ar40Nucleus`     4018
:sup:`39`\ K           :code:`K39Nucleus`      3919
:sup:`40`\ Ca          :code:`Ca40Nucleus`     4020
:sup:`45`\ Sc          :code:`Sc45Nucleus`     4521
:sup:`48`\ Ti          :code:`Ti48Nucleus`     4822
:sup:`51`\ V           :code:`V51Nucleus`      5123
:sup:`52`\ Cr          :code:`Cr52Nucleus`     5224
:sup:`55`\ Mn          :code:`Mn55Nucleus`     5525
:sup:`56`\ Fe          :code:`Fe56Nucleus`     5626
:math:`\gamma_{ch}`    :code:`CherenkovPhoton` 9900
====================== ======================= ====

Note that the code for nuclei follows the formula :math:`A\times100+Z`, where
*Z* is the charge and *A* is the mass number of the nucleus.

Physical Constants
------------------

The physics classes also define basic physical constants from the `Particle
Data Group <http://pdg.lbl.gov>`_.  They are defined in terms of the units
given in the :ref:`hawcnest` project and include the following values:

================== ================================ ============================ =======================
Constant           Value                            Units                        Description
================== ================================ ============================ =======================
:math:`c`          299792458                        m/s                          speed of light
:math:`\mu_0`      :math:`4\pi\times10^{-7}`        dyne/mA\ :sup:`2`            vacuum permeability
:math:`\epsilon_0` :math:`1/\mu_0 c^2`                                           vacuum permittivity
:math:`k_e`        1/:math:`4\pi\epsilon_0`                                      electric force constant
:math:`h`          :math:`6.62606896\times10^{-34}` J/s                          Planck's constant 
:math:`\hbar`      :math:`h/2\pi`                   J/s                          Reduced Planck const.
:math:`\alpha`     :math:`k_ee^2/(\hbar c)`                                      Fine structure constant
:math:`G`          :math:`6.67428\times10^{-11}`    m\ :sup:`3`/(kg s\ :sup:`2`) Gravitational constant
:math:`g`          :math:`9.80665`                  m/s\ :sup:`2`                Grav. accel. (standard)
:math:`N_a`        :math:`6.02214179\times10^{23}`  1/mol                        Avogadro's number
:math:`k_B`        :math:`1.3806504\times10^{-23}`  J/K                          Boltzmann's constant
:math:`R_g`        :math:`N_a\cdot k_B`                                          Molar gas constant
:math:`M_p`        :math:`938.272013`               MeV/c\ :sup:`2`              Proton mass
:math:`M_n`        :math:`939.565379`               MeV/c\ :sup:`2`              Neutron mass
:math:`M_d`        :math:`1875.612793`              MeV/c\ :sup:`2`              Deuteron mass
:math:`M_e`        :math:`0.510998910`              MeV/c\ :sup:`2`              Electron mass
:math:`M_\mu`      :math:`105.6583715`              MeV/c\ :sup:`2`              Muon mass
:math:`M_\tau`     :math:`1776.82`                  MeV/c\ :sup:`2`              Tau mass
:math:`R_e`        :math:`k_ee^2/(M_e c^2)`                                      Electron radius
:math:`R_b`        :math:`R_e/\alpha^2`                                          Bohr radius
:math:`\lambda_c`  :math:`h / (M_e c)`                                           Compton wavelength
:math:`R`          :math:`M_ec^2\alpha^2/2`                                      Rydberg constant
:math:`\mu_B`      :math:`e\hbar/(2M_e)`                                         Bohr magneton
:math:`\mu_N`      :math:`e\hbar/(2M_p)`                                         Nuclear magneton
================== ================================ ============================ =======================
