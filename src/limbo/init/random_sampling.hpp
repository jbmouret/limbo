#ifndef INITIALIZATION_FUNCTIONS_RANDOM_SAMPLING_HPP_
#define INITIALIZATION_FUNCTIONS_RANDOM_SAMPLING_HPP_

#include <Eigen/Core>
#include <limbo/tools/rand.hpp>

namespace limbo {
    namespace init {
        // initialize in [0,1] !
        // params: init::nb_samples
        template <typename Params>
        struct RandomSampling {
            template <typename F, typename Opt>
            void operator()(const F& feval, Opt& opt) const
            {
                for (int i = 0; i < Params::init::nb_samples(); i++) {
                    Eigen::VectorXd new_sample(F::dim_in);
                    for (int i = 0; i < F::dim_in; i++)
                        new_sample[i] = tools::rand<double>(0, 1);
                    std::cout << "random sample:" << new_sample.transpose() << std::endl;
                    opt.add_new_sample(new_sample, feval(new_sample));
                }
            }
        };
    }
}
#endif