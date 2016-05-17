#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_optimizers

#include <boost/test/unit_test.hpp>

#include <limbo/opt/chained.hpp>
#include <limbo/opt/cmaes.hpp>
#include <limbo/opt/grid_search.hpp>
#include <limbo/opt/random_point.hpp>
#include <limbo/tools/macros.hpp>

using namespace limbo;

struct Params {
    struct opt_gridsearch {
        BO_PARAM(int, bins, 20);
    };
};

// test with a standard function
int monodim_calls = 0;
opt::eval_t acqui_mono(const Eigen::VectorXd& v, bool eval_grad)
{
    assert(!eval_grad);
    monodim_calls++;
    return opt::no_grad(3 * v(0) + 5);
}

// test with a functor
int bidim_calls = 0;
struct FakeAcquiBi {
    opt::eval_t operator()(const Eigen::VectorXd& v, bool eval_grad) const
    {
        assert(!eval_grad);
        bidim_calls++;
        return opt::no_grad(3 * v(0) + 5 - 2 * v(1) - 5 * v(1) + 2);
    }
};

BOOST_AUTO_TEST_CASE(test_random_mono_dim)
{
    using namespace limbo;

    opt::RandomPoint<Params> optimizer;

    monodim_calls = 0;
    for (int i = 0; i < 1000; i++) {
        Eigen::VectorXd best_point = optimizer(acqui_mono, Eigen::VectorXd::Constant(1, 0.5), true);
        BOOST_CHECK_EQUAL(best_point.size(), 1);
        BOOST_CHECK(best_point(0) > 0 || std::abs(best_point(0)) < 1e-7);
        BOOST_CHECK(best_point(0) < 1 || std::abs(best_point(0) - 1) < 1e-7);
    }
}

BOOST_AUTO_TEST_CASE(test_random_bi_dim)
{
    using namespace limbo;

    opt::RandomPoint<Params> optimizer;

    bidim_calls = 0;
    for (int i = 0; i < 1000; i++) {
        Eigen::VectorXd best_point = optimizer(FakeAcquiBi(), Eigen::VectorXd::Constant(2, 0.5), true);
        BOOST_CHECK_EQUAL(best_point.size(), 2);
        BOOST_CHECK(best_point(0) > 0 || std::abs(best_point(0)) < 1e-7);
        BOOST_CHECK(best_point(0) < 1 || std::abs(best_point(0) - 1) < 1e-7);
        BOOST_CHECK(best_point(1) > 0 || std::abs(best_point(1)) < 1e-7);
        BOOST_CHECK(best_point(1) < 1 || std::abs(best_point(1) - 1) < 1e-7);
    }
}

BOOST_AUTO_TEST_CASE(test_grid_search_mono_dim)
{
    using namespace limbo;

    opt::GridSearch<Params> optimizer;

    monodim_calls = 0;
    Eigen::VectorXd best_point = optimizer(acqui_mono, Eigen::VectorXd::Constant(1, 0.5), true);

    BOOST_CHECK_EQUAL(best_point.size(), 1);
    BOOST_CHECK_CLOSE(best_point(0), 1, 0.0001);
    BOOST_CHECK_EQUAL(monodim_calls, Params::opt_gridsearch::bins() + 1);
}

BOOST_AUTO_TEST_CASE(test_grid_search_bi_dim)
{
    using namespace limbo;

    opt::GridSearch<Params> optimizer;

    bidim_calls = 0;
    Eigen::VectorXd best_point = optimizer(FakeAcquiBi(), Eigen::VectorXd::Constant(2, 0.5), true);

    BOOST_CHECK_EQUAL(best_point.size(), 2);
    BOOST_CHECK_CLOSE(best_point(0), 1, 0.0001);
    BOOST_CHECK_SMALL(best_point(1), 0.000001);
    // TO-DO: Maybe alter a little grid search so not to call more times the utility function
    BOOST_CHECK_EQUAL(bidim_calls, (Params::opt_gridsearch::bins() + 1) * (Params::opt_gridsearch::bins() + 1) + 21);
}

BOOST_AUTO_TEST_CASE(test_chained)
{
    using namespace limbo;

    typedef opt::GridSearch<Params> opt_1_t;
    typedef opt::RandomPoint<Params> opt_2_t;
    typedef opt::GridSearch<Params> opt_3_t;
    typedef opt::GridSearch<Params> opt_4_t;
    opt::Chained<Params, opt_1_t, opt_2_t, opt_3_t, opt_4_t> optimizer;

    monodim_calls = 0;
    Eigen::VectorXd best_point = optimizer(acqui_mono, Eigen::VectorXd::Constant(1, 0.5), true);

    BOOST_CHECK_EQUAL(best_point.size(), 1);
    BOOST_CHECK(best_point(0) > 0 || std::abs(best_point(0)) < 1e-7);
    BOOST_CHECK(best_point(0) < 1 || std::abs(best_point(0) - 1) < 1e-7);
    BOOST_CHECK_EQUAL(monodim_calls, (Params::opt_gridsearch::bins() + 1) * 3);
}
