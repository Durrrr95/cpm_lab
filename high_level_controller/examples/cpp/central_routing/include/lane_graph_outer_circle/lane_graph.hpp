#pragma once

// Generated by map_print/map_print2/lane_graph_to_cpp.m
// Do not edit
#include <vector>

/**
 * \defgroup lane_graph_outer_circle
 * \ingroup central_routing
 * TODO
 */
/**
 * \struct LaneGraph
 * \brief TODO
 * \ingroup lane_graph_outer_circle
 */
struct LaneGraph {
    const size_t n_nodes = 16;
    const std::vector<double> nodes_x = std::vector<double>{2.2500e+00,3.1575e+00,3.8795e+00,4.3584e+00,4.3950e+00,4.3584e+00,3.8795e+00,3.1575e+00,2.2500e+00,1.3425e+00,6.2050e-01,1.4158e-01,1.0500e-01,1.4158e-01,6.2050e-01,1.3425e+00,};
    const std::vector<double> nodes_y = std::vector<double>{3.8950e+00,3.8888e+00,3.7217e+00,2.9549e+00,2.0000e+00,1.0451e+00,2.7828e-01,1.1119e-01,1.0500e-01,1.1119e-01,2.7828e-01,1.0451e+00,2.0000e+00,2.9549e+00,3.7217e+00,3.8888e+00,};
    const std::vector<double> nodes_cos = std::vector<double>{1.0000e+00,9.9874e-01,8.7680e-01,1.5927e-01,0.0000e+00,-1.5927e-01,-8.7680e-01,-9.9874e-01,-1.0000e+00,-9.9874e-01,-8.7680e-01,-1.5927e-01,0.0000e+00,1.5927e-01,8.7680e-01,9.9874e-01,};
    const std::vector<double> nodes_sin = std::vector<double>{0.0000e+00,-5.0154e-02,-4.8086e-01,-9.8724e-01,-1.0000e+00,-9.8724e-01,-4.8086e-01,-5.0154e-02,0.0000e+00,5.0154e-02,4.8086e-01,9.8724e-01,1.0000e+00,9.8724e-01,4.8086e-01,5.0154e-02,};
    const size_t n_edges = 16;
    const size_t n_edge_path_nodes = 25;
    const std::vector<size_t> edges_start_index = std::vector<size_t>{0,1,2,3,15,14,13,12,7,6,5,4,8,9,10,11,};
    const std::vector<size_t> edges_end_index = std::vector<size_t>{1,2,3,4,0,15,14,13,8,7,6,5,9,10,11,12,};

    const std::vector<std::vector<double>> edges_x = std::vector<std::vector<double>>
    {
        std::vector<double>{2.2500e+00,2.2874e+00,2.3248e+00,2.3626e+00,2.4000e+00,2.4373e+00,2.4747e+00,2.5121e+00,2.5495e+00,2.5873e+00,2.6247e+00,2.6622e+00,2.6997e+00,2.7373e+00,2.7749e+00,2.8130e+00,2.8508e+00,2.8887e+00,2.9267e+00,2.9648e+00,3.0030e+00,3.0417e+00,3.0801e+00,3.1188e+00,3.1575e+00,},
        std::vector<double>{3.1575e+00,3.1868e+00,3.2165e+00,3.2460e+00,3.2759e+00,3.3056e+00,3.3357e+00,3.3655e+00,3.3954e+00,3.4258e+00,3.4559e+00,3.4864e+00,3.5167e+00,3.5469e+00,3.5777e+00,3.6080e+00,3.6387e+00,3.6691e+00,3.6997e+00,3.7300e+00,3.7601e+00,3.7905e+00,3.8203e+00,3.8502e+00,3.8795e+00,},
        std::vector<double>{3.8795e+00,3.9179e+00,3.9559e+00,3.9926e+00,4.0282e+00,4.0625e+00,4.0927e+00,4.1191e+00,4.1439e+00,4.1670e+00,4.1887e+00,4.2087e+00,4.2272e+00,4.2444e+00,4.2603e+00,4.2747e+00,4.2880e+00,4.3001e+00,4.3112e+00,4.3213e+00,4.3303e+00,4.3385e+00,4.3459e+00,4.3525e+00,4.3584e+00,},
        std::vector<double>{4.3584e+00,4.3647e+00,4.3701e+00,4.3747e+00,4.3786e+00,4.3819e+00,4.3847e+00,4.3870e+00,4.3889e+00,4.3904e+00,4.3917e+00,4.3926e+00,4.3934e+00,4.3939e+00,4.3943e+00,4.3946e+00,4.3948e+00,4.3949e+00,4.3950e+00,4.3950e+00,4.3950e+00,4.3950e+00,4.3950e+00,4.3950e+00,4.3950e+00,},
        std::vector<double>{1.3425e+00,1.3812e+00,1.4199e+00,1.4587e+00,1.4970e+00,1.5352e+00,1.5733e+00,1.6113e+00,1.6492e+00,1.6873e+00,1.7251e+00,1.7627e+00,1.8003e+00,1.8378e+00,1.8753e+00,1.9131e+00,1.9505e+00,1.9879e+00,2.0253e+00,2.0627e+00,2.1000e+00,2.1378e+00,2.1752e+00,2.2126e+00,2.2500e+00,},
        std::vector<double>{6.2050e-01,6.4979e-01,6.7974e-01,7.0954e-01,7.3990e-01,7.7002e-01,8.0063e-01,8.3093e-01,8.6127e-01,8.9200e-01,9.2235e-01,9.5305e-01,9.8334e-01,1.0136e+00,1.0441e+00,1.0742e+00,1.1046e+00,1.1345e+00,1.1647e+00,1.1944e+00,1.2241e+00,1.2540e+00,1.2835e+00,1.3132e+00,1.3425e+00,},
        std::vector<double>{1.4158e-01,1.4746e-01,1.5412e-01,1.6149e-01,1.6967e-01,1.7874e-01,1.8885e-01,1.9989e-01,2.1200e-01,2.2526e-01,2.3988e-01,2.5565e-01,2.7276e-01,2.9129e-01,3.1148e-01,3.3302e-01,3.5614e-01,3.8088e-01,4.0754e-01,4.3753e-01,4.7180e-01,5.0740e-01,5.4447e-01,5.8211e-01,6.2050e-01,},
        std::vector<double>{1.0500e-01,1.0500e-01,1.0500e-01,1.0500e-01,1.0500e-01,1.0501e-01,1.0505e-01,1.0511e-01,1.0523e-01,1.0542e-01,1.0570e-01,1.0610e-01,1.0665e-01,1.0738e-01,1.0834e-01,1.0955e-01,1.1108e-01,1.1298e-01,1.1529e-01,1.1805e-01,1.2138e-01,1.2532e-01,1.2996e-01,1.3532e-01,1.4158e-01,},
        std::vector<double>{3.1575e+00,3.1188e+00,3.0801e+00,3.0413e+00,3.0030e+00,2.9648e+00,2.9267e+00,2.8887e+00,2.8508e+00,2.8127e+00,2.7749e+00,2.7373e+00,2.6997e+00,2.6622e+00,2.6247e+00,2.5869e+00,2.5495e+00,2.5121e+00,2.4747e+00,2.4373e+00,2.4000e+00,2.3622e+00,2.3248e+00,2.2874e+00,2.2500e+00,},
        std::vector<double>{3.8795e+00,3.8502e+00,3.8203e+00,3.7905e+00,3.7601e+00,3.7300e+00,3.6994e+00,3.6691e+00,3.6387e+00,3.6080e+00,3.5777e+00,3.5469e+00,3.5167e+00,3.4864e+00,3.4559e+00,3.4258e+00,3.3954e+00,3.3655e+00,3.3353e+00,3.3056e+00,3.2759e+00,3.2460e+00,3.2165e+00,3.1868e+00,3.1575e+00,},
        std::vector<double>{4.3584e+00,4.3525e+00,4.3459e+00,4.3385e+00,4.3303e+00,4.3213e+00,4.3111e+00,4.3001e+00,4.2880e+00,4.2747e+00,4.2601e+00,4.2444e+00,4.2272e+00,4.2087e+00,4.1885e+00,4.1670e+00,4.1439e+00,4.1191e+00,4.0925e+00,4.0625e+00,4.0282e+00,3.9926e+00,3.9555e+00,3.9179e+00,3.8795e+00,},
        std::vector<double>{4.3950e+00,4.3950e+00,4.3950e+00,4.3950e+00,4.3950e+00,4.3950e+00,4.3950e+00,4.3949e+00,4.3948e+00,4.3946e+00,4.3943e+00,4.3939e+00,4.3934e+00,4.3926e+00,4.3917e+00,4.3904e+00,4.3889e+00,4.3870e+00,4.3847e+00,4.3819e+00,4.3786e+00,4.3747e+00,4.3700e+00,4.3647e+00,4.3584e+00,},
        std::vector<double>{2.2500e+00,2.2126e+00,2.1752e+00,2.1374e+00,2.1000e+00,2.0627e+00,2.0253e+00,1.9879e+00,1.9505e+00,1.9127e+00,1.8753e+00,1.8378e+00,1.8003e+00,1.7627e+00,1.7251e+00,1.6870e+00,1.6492e+00,1.6113e+00,1.5733e+00,1.5352e+00,1.4970e+00,1.4583e+00,1.4199e+00,1.3812e+00,1.3425e+00,},
        std::vector<double>{1.3425e+00,1.3132e+00,1.2835e+00,1.2540e+00,1.2241e+00,1.1944e+00,1.1643e+00,1.1345e+00,1.1046e+00,1.0742e+00,1.0441e+00,1.0136e+00,9.8334e-01,9.5305e-01,9.2235e-01,8.9200e-01,8.6127e-01,8.3093e-01,8.0025e-01,7.7002e-01,7.3990e-01,7.0954e-01,6.7974e-01,6.4979e-01,6.2050e-01,},
        std::vector<double>{6.2050e-01,5.8211e-01,5.4411e-01,5.0740e-01,4.7180e-01,4.3753e-01,4.0728e-01,3.8088e-01,3.5614e-01,3.3302e-01,3.1128e-01,2.9129e-01,2.7276e-01,2.5565e-01,2.3974e-01,2.2526e-01,2.1200e-01,1.9989e-01,1.8875e-01,1.7874e-01,1.6967e-01,1.6149e-01,1.5406e-01,1.4746e-01,1.4158e-01,},
        std::vector<double>{1.4158e-01,1.3532e-01,1.2992e-01,1.2532e-01,1.2138e-01,1.1805e-01,1.1527e-01,1.1298e-01,1.1108e-01,1.0955e-01,1.0833e-01,1.0738e-01,1.0665e-01,1.0610e-01,1.0570e-01,1.0542e-01,1.0523e-01,1.0511e-01,1.0504e-01,1.0501e-01,1.0500e-01,1.0500e-01,1.0500e-01,1.0500e-01,1.0500e-01,},
    };

    const std::vector<std::vector<double>> edges_y = std::vector<std::vector<double>>
    {
        std::vector<double>{3.8950e+00,3.8950e+00,3.8950e+00,3.8950e+00,3.8951e+00,3.8951e+00,3.8952e+00,3.8953e+00,3.8954e+00,3.8955e+00,3.8956e+00,3.8957e+00,3.8958e+00,3.8959e+00,3.8959e+00,3.8959e+00,3.8957e+00,3.8955e+00,3.8952e+00,3.8947e+00,3.8940e+00,3.8931e+00,3.8920e+00,3.8906e+00,3.8888e+00,},
        std::vector<double>{3.8888e+00,3.8872e+00,3.8854e+00,3.8833e+00,3.8809e+00,3.8782e+00,3.8752e+00,3.8718e+00,3.8680e+00,3.8637e+00,3.8590e+00,3.8537e+00,3.8480e+00,3.8417e+00,3.8347e+00,3.8271e+00,3.8187e+00,3.8097e+00,3.7998e+00,3.7892e+00,3.7777e+00,3.7651e+00,3.7517e+00,3.7372e+00,3.7217e+00,},
        std::vector<double>{3.7217e+00,3.6995e+00,3.6750e+00,3.6487e+00,3.6201e+00,3.5893e+00,3.5589e+00,3.5294e+00,3.4989e+00,3.4675e+00,3.4351e+00,3.4024e+00,3.3692e+00,3.3355e+00,3.3012e+00,3.2670e+00,3.2325e+00,3.1980e+00,3.1630e+00,3.1283e+00,3.0936e+00,3.0589e+00,3.0239e+00,2.9893e+00,2.9549e+00,},
        std::vector<double>{2.9549e+00,2.9133e+00,2.8719e+00,2.8310e+00,2.7899e+00,2.7491e+00,2.7084e+00,2.6683e+00,2.6280e+00,2.5878e+00,2.5479e+00,2.5084e+00,2.4687e+00,2.4291e+00,2.3897e+00,2.3507e+00,2.3114e+00,2.2723e+00,2.2332e+00,2.1944e+00,2.1554e+00,2.1165e+00,2.0776e+00,2.0389e+00,2.0000e+00,},
        std::vector<double>{3.8888e+00,3.8906e+00,3.8920e+00,3.8932e+00,3.8940e+00,3.8947e+00,3.8952e+00,3.8955e+00,3.8957e+00,3.8959e+00,3.8959e+00,3.8959e+00,3.8958e+00,3.8957e+00,3.8956e+00,3.8955e+00,3.8954e+00,3.8953e+00,3.8952e+00,3.8951e+00,3.8951e+00,3.8950e+00,3.8950e+00,3.8950e+00,3.8950e+00,},
        std::vector<double>{3.7217e+00,3.7372e+00,3.7517e+00,3.7651e+00,3.7777e+00,3.7892e+00,3.7999e+00,3.8097e+00,3.8187e+00,3.8271e+00,3.8347e+00,3.8417e+00,3.8480e+00,3.8537e+00,3.8590e+00,3.8637e+00,3.8680e+00,3.8718e+00,3.8752e+00,3.8782e+00,3.8809e+00,3.8833e+00,3.8854e+00,3.8872e+00,3.8888e+00,},
        std::vector<double>{2.9549e+00,2.9893e+00,3.0242e+00,3.0589e+00,3.0936e+00,3.1283e+00,3.1633e+00,3.1980e+00,3.2325e+00,3.2670e+00,3.3016e+00,3.3355e+00,3.3692e+00,3.4024e+00,3.4354e+00,3.4675e+00,3.4989e+00,3.5294e+00,3.5592e+00,3.5893e+00,3.6201e+00,3.6487e+00,3.6753e+00,3.6995e+00,3.7217e+00,},
        std::vector<double>{2.0000e+00,2.0389e+00,2.0779e+00,2.1165e+00,2.1554e+00,2.1944e+00,2.2335e+00,2.2723e+00,2.3114e+00,2.3507e+00,2.3900e+00,2.4291e+00,2.4687e+00,2.5084e+00,2.5482e+00,2.5878e+00,2.6280e+00,2.6683e+00,2.7088e+00,2.7491e+00,2.7899e+00,2.8310e+00,2.8722e+00,2.9133e+00,2.9549e+00,},
        std::vector<double>{1.1119e-01,1.0942e-01,1.0799e-01,1.0684e-01,1.0596e-01,1.0529e-01,1.0481e-01,1.0447e-01,1.0426e-01,1.0415e-01,1.0412e-01,1.0414e-01,1.0421e-01,1.0430e-01,1.0441e-01,1.0452e-01,1.0463e-01,1.0473e-01,1.0482e-01,1.0489e-01,1.0494e-01,1.0497e-01,1.0499e-01,1.0500e-01,1.0500e-01,},
        std::vector<double>{2.7828e-01,2.6284e-01,2.4827e-01,2.3490e-01,2.2233e-01,2.1084e-01,2.0008e-01,1.9029e-01,1.8128e-01,1.7290e-01,1.6533e-01,1.5832e-01,1.5201e-01,1.4628e-01,1.4102e-01,1.3632e-01,1.3204e-01,1.2824e-01,1.2480e-01,1.2178e-01,1.1909e-01,1.1669e-01,1.1461e-01,1.1276e-01,1.1119e-01,},
        std::vector<double>{1.0451e+00,1.0107e+00,9.7577e-01,9.4113e-01,9.0644e-01,8.7172e-01,8.3669e-01,8.0203e-01,7.6745e-01,7.3301e-01,6.9845e-01,6.6447e-01,6.3082e-01,5.9760e-01,5.6457e-01,5.3248e-01,5.0112e-01,4.7062e-01,4.4083e-01,4.1066e-01,3.7987e-01,3.5133e-01,3.2472e-01,3.0049e-01,2.7828e-01,},
        std::vector<double>{2.0000e+00,1.9611e+00,1.9221e+00,1.8835e+00,1.8446e+00,1.8056e+00,1.7665e+00,1.7277e+00,1.6886e+00,1.6493e+00,1.6100e+00,1.5709e+00,1.5313e+00,1.4916e+00,1.4518e+00,1.4122e+00,1.3720e+00,1.3317e+00,1.2912e+00,1.2509e+00,1.2101e+00,1.1690e+00,1.1278e+00,1.0867e+00,1.0451e+00,},
        std::vector<double>{1.0500e-01,1.0500e-01,1.0499e-01,1.0497e-01,1.0494e-01,1.0489e-01,1.0482e-01,1.0473e-01,1.0463e-01,1.0452e-01,1.0441e-01,1.0430e-01,1.0421e-01,1.0414e-01,1.0412e-01,1.0415e-01,1.0426e-01,1.0447e-01,1.0481e-01,1.0529e-01,1.0596e-01,1.0685e-01,1.0799e-01,1.0942e-01,1.1119e-01,},
        std::vector<double>{1.1119e-01,1.1276e-01,1.1461e-01,1.1669e-01,1.1909e-01,1.2178e-01,1.2484e-01,1.2824e-01,1.3204e-01,1.3632e-01,1.4102e-01,1.4628e-01,1.5201e-01,1.5832e-01,1.6533e-01,1.7290e-01,1.8128e-01,1.9029e-01,2.0021e-01,2.1084e-01,2.2233e-01,2.3490e-01,2.4827e-01,2.6284e-01,2.7828e-01,},
        std::vector<double>{2.7828e-01,3.0049e-01,3.2496e-01,3.5133e-01,3.7987e-01,4.1066e-01,4.4110e-01,4.7062e-01,5.0112e-01,5.3248e-01,5.6488e-01,5.9760e-01,6.3082e-01,6.6447e-01,6.9877e-01,7.3301e-01,7.6745e-01,8.0203e-01,8.3702e-01,8.7172e-01,9.0644e-01,9.4113e-01,9.7610e-01,1.0107e+00,1.0451e+00,},
        std::vector<double>{1.0451e+00,1.0867e+00,1.1281e+00,1.1690e+00,1.2101e+00,1.2509e+00,1.2916e+00,1.3317e+00,1.3720e+00,1.4122e+00,1.4521e+00,1.4916e+00,1.5313e+00,1.5709e+00,1.6103e+00,1.6493e+00,1.6886e+00,1.7277e+00,1.7668e+00,1.8056e+00,1.8446e+00,1.8835e+00,1.9224e+00,1.9611e+00,2.0000e+00,},
    };

    const std::vector<std::vector<double>> edges_cos = std::vector<std::vector<double>>
    {
        std::vector<double>{1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,9.9999e-01,9.9998e-01,9.9994e-01,9.9989e-01,9.9980e-01,9.9966e-01,9.9945e-01,9.9916e-01,9.9875e-01,},
        std::vector<double>{9.9874e-01,9.9834e-01,9.9782e-01,9.9718e-01,9.9639e-01,9.9543e-01,9.9425e-01,9.9286e-01,9.9119e-01,9.8918e-01,9.8684e-01,9.8405e-01,9.8083e-01,9.7709e-01,9.7269e-01,9.6768e-01,9.6184e-01,9.5525e-01,9.4764e-01,9.3909e-01,9.2943e-01,9.1839e-01,9.0613e-01,8.9223e-01,8.7689e-01,},
        std::vector<double>{8.7668e-01,8.5381e-01,8.2711e-01,7.9696e-01,7.6281e-01,7.2437e-01,6.8524e-01,6.4841e-01,6.1169e-01,5.7533e-01,5.3922e-01,5.0421e-01,4.7011e-01,4.3707e-01,4.0489e-01,3.7425e-01,3.4491e-01,3.1691e-01,2.9005e-01,2.6484e-01,2.4102e-01,2.1860e-01,1.9736e-01,1.7769e-01,1.5934e-01,},
        std::vector<double>{1.5917e-01,1.3887e-01,1.2022e-01,1.0346e-01,8.8219e-02,7.4569e-02,6.2418e-02,5.1759e-02,4.2332e-02,3.4137e-02,2.7085e-02,2.1129e-02,1.6086e-02,1.1920e-02,8.5433e-03,5.8890e-03,3.8309e-03,2.3096e-03,1.2433e-03,5.5588e-04,1.5791e-04,-2.1301e-05,-5.7829e-05,-2.6650e-05,-7.9530e-10,},
        std::vector<double>{9.9875e-01,9.9916e-01,9.9945e-01,9.9966e-01,9.9980e-01,9.9989e-01,9.9994e-01,9.9998e-01,9.9999e-01,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,},
        std::vector<double>{8.7689e-01,8.9223e-01,9.0613e-01,9.1839e-01,9.2943e-01,9.3909e-01,9.4774e-01,9.5525e-01,9.6184e-01,9.6768e-01,9.7269e-01,9.7709e-01,9.8083e-01,9.8405e-01,9.8684e-01,9.8918e-01,9.9119e-01,9.9286e-01,9.9427e-01,9.9543e-01,9.9639e-01,9.9718e-01,9.9782e-01,9.9834e-01,9.9874e-01,},
        std::vector<double>{1.5934e-01,1.7769e-01,1.9756e-01,2.1860e-01,2.4102e-01,2.6484e-01,2.9030e-01,3.1691e-01,3.4491e-01,3.7425e-01,4.0518e-01,4.3707e-01,4.7011e-01,5.0421e-01,5.3955e-01,5.7533e-01,6.1169e-01,6.4841e-01,6.8559e-01,7.2437e-01,7.6281e-01,7.9696e-01,8.2737e-01,8.5381e-01,8.7668e-01,},
        std::vector<double>{-7.9530e-10,-2.6650e-05,-5.7905e-05,-2.1301e-05,1.5791e-04,5.5588e-04,1.2502e-03,2.3096e-03,3.8309e-03,5.8890e-03,8.5670e-03,1.1920e-02,1.6086e-02,2.1129e-02,2.7136e-02,3.4137e-02,4.2332e-02,5.1759e-02,6.2508e-02,7.4569e-02,8.8219e-02,1.0346e-01,1.2036e-01,1.3887e-01,1.5917e-01,},
        std::vector<double>{-9.9875e-01,-9.9916e-01,-9.9945e-01,-9.9966e-01,-9.9980e-01,-9.9989e-01,-9.9994e-01,-9.9998e-01,-9.9999e-01,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,},
        std::vector<double>{-8.7689e-01,-8.9223e-01,-9.0613e-01,-9.1839e-01,-9.2943e-01,-9.3909e-01,-9.4774e-01,-9.5525e-01,-9.6184e-01,-9.6768e-01,-9.7269e-01,-9.7709e-01,-9.8083e-01,-9.8405e-01,-9.8684e-01,-9.8918e-01,-9.9119e-01,-9.9286e-01,-9.9427e-01,-9.9543e-01,-9.9639e-01,-9.9718e-01,-9.9782e-01,-9.9834e-01,-9.9874e-01,},
        std::vector<double>{-1.5934e-01,-1.7769e-01,-1.9756e-01,-2.1860e-01,-2.4102e-01,-2.6484e-01,-2.9030e-01,-3.1691e-01,-3.4491e-01,-3.7425e-01,-4.0518e-01,-4.3707e-01,-4.7011e-01,-5.0421e-01,-5.3955e-01,-5.7533e-01,-6.1169e-01,-6.4841e-01,-6.8559e-01,-7.2437e-01,-7.6281e-01,-7.9696e-01,-8.2737e-01,-8.5381e-01,-8.7668e-01,},
        std::vector<double>{7.9530e-10,2.6650e-05,5.7905e-05,2.1301e-05,-1.5791e-04,-5.5588e-04,-1.2502e-03,-2.3096e-03,-3.8309e-03,-5.8890e-03,-8.5670e-03,-1.1920e-02,-1.6086e-02,-2.1129e-02,-2.7136e-02,-3.4137e-02,-4.2332e-02,-5.1759e-02,-6.2508e-02,-7.4569e-02,-8.8219e-02,-1.0346e-01,-1.2036e-01,-1.3887e-01,-1.5917e-01,},
        std::vector<double>{-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-9.9999e-01,-9.9998e-01,-9.9994e-01,-9.9989e-01,-9.9980e-01,-9.9966e-01,-9.9945e-01,-9.9916e-01,-9.9875e-01,},
        std::vector<double>{-9.9874e-01,-9.9834e-01,-9.9782e-01,-9.9718e-01,-9.9639e-01,-9.9543e-01,-9.9425e-01,-9.9286e-01,-9.9119e-01,-9.8918e-01,-9.8684e-01,-9.8405e-01,-9.8083e-01,-9.7709e-01,-9.7269e-01,-9.6768e-01,-9.6184e-01,-9.5525e-01,-9.4764e-01,-9.3909e-01,-9.2943e-01,-9.1839e-01,-9.0613e-01,-8.9223e-01,-8.7689e-01,},
        std::vector<double>{-8.7668e-01,-8.5381e-01,-8.2711e-01,-7.9696e-01,-7.6281e-01,-7.2437e-01,-6.8524e-01,-6.4841e-01,-6.1169e-01,-5.7533e-01,-5.3922e-01,-5.0421e-01,-4.7011e-01,-4.3707e-01,-4.0489e-01,-3.7425e-01,-3.4491e-01,-3.1691e-01,-2.9005e-01,-2.6484e-01,-2.4102e-01,-2.1860e-01,-1.9736e-01,-1.7769e-01,-1.5934e-01,},
        std::vector<double>{-1.5917e-01,-1.3887e-01,-1.2022e-01,-1.0346e-01,-8.8219e-02,-7.4569e-02,-6.2418e-02,-5.1759e-02,-4.2332e-02,-3.4137e-02,-2.7085e-02,-2.1129e-02,-1.6086e-02,-1.1920e-02,-8.5433e-03,-5.8890e-03,-3.8309e-03,-2.3096e-03,-1.2433e-03,-5.5588e-04,-1.5791e-04,2.1301e-05,5.7829e-05,2.6650e-05,7.9530e-10,},
    };

    const std::vector<std::vector<double>> edges_sin = std::vector<std::vector<double>>
    {
        std::vector<double>{2.8280e-09,8.7033e-05,3.2684e-04,6.8721e-04,1.1230e-03,1.5971e-03,2.0674e-03,2.4902e-03,2.8192e-03,3.0077e-03,3.0017e-03,2.7517e-03,2.2038e-03,1.3022e-03,-1.1162e-05,-1.8156e-03,-4.1394e-03,-7.0609e-03,-1.0646e-02,-1.4962e-02,-2.0079e-02,-2.6128e-02,-3.3068e-02,-4.1027e-02,-5.0078e-02,},
        std::vector<double>{-5.0170e-02,-5.7655e-02,-6.6035e-02,-7.5042e-02,-8.4936e-02,-9.5504e-02,-1.0704e-01,-1.1930e-01,-1.3244e-01,-1.4668e-01,-1.6169e-01,-1.7788e-01,-1.9486e-01,-2.1285e-01,-2.3212e-01,-2.5220e-01,-2.7360e-01,-2.9580e-01,-3.1935e-01,-3.4366e-01,-3.6900e-01,-3.9568e-01,-4.2299e-01,-4.5159e-01,-4.8070e-01,},
        std::vector<double>{-4.8107e-01,-5.2058e-01,-5.6205e-01,-6.0403e-01,-6.4662e-01,-6.8942e-01,-7.2831e-01,-7.6129e-01,-7.9110e-01,-8.1792e-01,-8.4217e-01,-8.6358e-01,-8.8261e-01,-8.9943e-01,-9.1437e-01,-9.2733e-01,-9.3864e-01,-9.4845e-01,-9.5701e-01,-9.6429e-01,-9.7052e-01,-9.7581e-01,-9.8033e-01,-9.8409e-01,-9.8722e-01,},
        std::vector<double>{-9.8725e-01,-9.9031e-01,-9.9275e-01,-9.9463e-01,-9.9610e-01,-9.9722e-01,-9.9805e-01,-9.9866e-01,-9.9910e-01,-9.9942e-01,-9.9963e-01,-9.9978e-01,-9.9987e-01,-9.9993e-01,-9.9996e-01,-9.9998e-01,-9.9999e-01,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,},
        std::vector<double>{5.0078e-02,4.1027e-02,3.3068e-02,2.6066e-02,2.0079e-02,1.4962e-02,1.0646e-02,7.0609e-03,4.1394e-03,1.7961e-03,1.1162e-05,-1.3022e-03,-2.2038e-03,-2.7517e-03,-3.0017e-03,-3.0068e-03,-2.8192e-03,-2.4902e-03,-2.0674e-03,-1.5971e-03,-1.1230e-03,-6.8337e-04,-3.2684e-04,-8.7033e-05,-2.8280e-09,},
        std::vector<double>{4.8070e-01,4.5159e-01,4.2299e-01,3.9568e-01,3.6900e-01,3.4366e-01,3.1905e-01,2.9580e-01,2.7360e-01,2.5220e-01,2.3212e-01,2.1285e-01,1.9486e-01,1.7788e-01,1.6169e-01,1.4668e-01,1.3244e-01,1.1930e-01,1.0689e-01,9.5504e-02,8.4936e-02,7.5042e-02,6.6035e-02,5.7655e-02,5.0170e-02,},
        std::vector<double>{9.8722e-01,9.8409e-01,9.8029e-01,9.7581e-01,9.7052e-01,9.6429e-01,9.5694e-01,9.4845e-01,9.3864e-01,9.2733e-01,9.1424e-01,8.9943e-01,8.8261e-01,8.6358e-01,8.4195e-01,8.1792e-01,7.9110e-01,7.6129e-01,7.2798e-01,6.8942e-01,6.4662e-01,6.0403e-01,5.6165e-01,5.2058e-01,4.8107e-01,},
        std::vector<double>{1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,9.9999e-01,9.9998e-01,9.9996e-01,9.9993e-01,9.9987e-01,9.9978e-01,9.9963e-01,9.9942e-01,9.9910e-01,9.9866e-01,9.9804e-01,9.9722e-01,9.9610e-01,9.9463e-01,9.9273e-01,9.9031e-01,9.8725e-01,},
        std::vector<double>{-5.0078e-02,-4.1027e-02,-3.3068e-02,-2.6066e-02,-2.0079e-02,-1.4962e-02,-1.0646e-02,-7.0609e-03,-4.1394e-03,-1.7961e-03,-1.1162e-05,1.3022e-03,2.2038e-03,2.7517e-03,3.0017e-03,3.0068e-03,2.8192e-03,2.4902e-03,2.0674e-03,1.5971e-03,1.1230e-03,6.8337e-04,3.2684e-04,8.7033e-05,2.8280e-09,},
        std::vector<double>{-4.8070e-01,-4.5159e-01,-4.2299e-01,-3.9568e-01,-3.6900e-01,-3.4366e-01,-3.1905e-01,-2.9580e-01,-2.7360e-01,-2.5220e-01,-2.3212e-01,-2.1285e-01,-1.9486e-01,-1.7788e-01,-1.6169e-01,-1.4668e-01,-1.3244e-01,-1.1930e-01,-1.0689e-01,-9.5504e-02,-8.4936e-02,-7.5042e-02,-6.6035e-02,-5.7655e-02,-5.0170e-02,},
        std::vector<double>{-9.8722e-01,-9.8409e-01,-9.8029e-01,-9.7581e-01,-9.7052e-01,-9.6429e-01,-9.5694e-01,-9.4845e-01,-9.3864e-01,-9.2733e-01,-9.1424e-01,-8.9943e-01,-8.8261e-01,-8.6358e-01,-8.4195e-01,-8.1792e-01,-7.9110e-01,-7.6129e-01,-7.2798e-01,-6.8942e-01,-6.4662e-01,-6.0403e-01,-5.6165e-01,-5.2058e-01,-4.8107e-01,},
        std::vector<double>{-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-1.0000e+00,-9.9999e-01,-9.9998e-01,-9.9996e-01,-9.9993e-01,-9.9987e-01,-9.9978e-01,-9.9963e-01,-9.9942e-01,-9.9910e-01,-9.9866e-01,-9.9804e-01,-9.9722e-01,-9.9610e-01,-9.9463e-01,-9.9273e-01,-9.9031e-01,-9.8725e-01,},
        std::vector<double>{-2.8280e-09,-8.7033e-05,-3.2684e-04,-6.8721e-04,-1.1230e-03,-1.5971e-03,-2.0674e-03,-2.4902e-03,-2.8192e-03,-3.0077e-03,-3.0017e-03,-2.7517e-03,-2.2038e-03,-1.3022e-03,1.1162e-05,1.8156e-03,4.1394e-03,7.0609e-03,1.0646e-02,1.4962e-02,2.0079e-02,2.6128e-02,3.3068e-02,4.1027e-02,5.0078e-02,},
        std::vector<double>{5.0170e-02,5.7655e-02,6.6035e-02,7.5042e-02,8.4936e-02,9.5504e-02,1.0704e-01,1.1930e-01,1.3244e-01,1.4668e-01,1.6169e-01,1.7788e-01,1.9486e-01,2.1285e-01,2.3212e-01,2.5220e-01,2.7360e-01,2.9580e-01,3.1935e-01,3.4366e-01,3.6900e-01,3.9568e-01,4.2299e-01,4.5159e-01,4.8070e-01,},
        std::vector<double>{4.8107e-01,5.2058e-01,5.6205e-01,6.0403e-01,6.4662e-01,6.8942e-01,7.2831e-01,7.6129e-01,7.9110e-01,8.1792e-01,8.4217e-01,8.6358e-01,8.8261e-01,8.9943e-01,9.1437e-01,9.2733e-01,9.3864e-01,9.4845e-01,9.5701e-01,9.6429e-01,9.7052e-01,9.7581e-01,9.8033e-01,9.8409e-01,9.8722e-01,},
        std::vector<double>{9.8725e-01,9.9031e-01,9.9275e-01,9.9463e-01,9.9610e-01,9.9722e-01,9.9805e-01,9.9866e-01,9.9910e-01,9.9942e-01,9.9963e-01,9.9978e-01,9.9987e-01,9.9993e-01,9.9996e-01,9.9998e-01,9.9999e-01,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,1.0000e+00,},
    };

};
