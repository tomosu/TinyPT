# TinyPT
TinyPT is a simple photon transport code for me, a quiche eater.
## Example1: Cone beam CT simulation
![Alt Text](https://github.com/tomosu/TinyPT/raw/master/Image/attenuationImage.gif)
## Example2: Photon scattering angle calculation
```
Compound Name: CsI
Density: 4.510000e+00

Elements Info
1:
ElementName: Cesium(Z=55)
AtomicWeight: 1.329050e+02
K_alpha(MeV): 3.097708e-02
Quantity: 1

2:
ElementName: Iodine(Z=53)
AtomicWeight: 1.269044e+02
K_alpha(MeV): 2.861597e-02
Quantity: 1

Calc Rayleigh scattering angle distribution @0.04MeV

probability
7.51e-02 #   +
         #  + +
         #     +
         #      +
         # +     +
         #        +
         #         +
4.00e-02 #          +
         #           +
         #            ++
         #              +
         #+              ++
         #                 ++
         #                   +++
5.00e-03 #+                     ++++++++++++++++++++++++++ +
         ###################################################
         0.00e+00                 8.82e+01                 1.76e+02
                                                          degree

Calc Compton scattering angle distribution @0.04MeV

probability
2.75e-02 #                                  +
         #              ++++++++++   +++++++ +++
         #            ++          +++           ++
         #           +                            +
         #          +                              +
         #         +                                +
         #        +                                  +
1.47e-02 #       +                                    +
         #                                             +
         #      +
         #     +                                        +
         #                                               +
         #    +
         #   +                                             +
1.83e-03 #+++
         ###################################################
         0.00e+00                 8.82e+01                 1.76e+02
                                                          degree
                                                          ```
