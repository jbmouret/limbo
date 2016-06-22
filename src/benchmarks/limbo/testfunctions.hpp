#define _USE_MATH_DEFINES
#include <Eigen/Core>

// support functions
inline double sign(double x)
{
    if (x < 0)
        return -1;
    if (x > 0)
        return 1;
    return 0;
}

inline double sqr(double x)
{
    return x * x;
};

inline double hat(double x)
{
    if (x != 0)
        return log(fabs(x));
    return 0;
}

inline double c1(double x)
{
    if (x > 0)
        return 10;
    return 5.5;
}

inline double c2(double x)
{
    if (x > 0)
        return 7.9;
    return 3.1;
}

inline Eigen::VectorXd t_osz(const Eigen::VectorXd& x)
{
    Eigen::VectorXd r = x;
    for (int i = 0; i < x.size(); i++)
        r(i) = sign(x(i)) * exp(hat(x(i)) + 0.049 * sin(c1(x(i)) * hat(x(i))) + sin(c2(x(i)) * hat(x(i))));
    return r;
}

struct Sphere {
    static constexpr size_t dim_in = 2;
    static constexpr size_t dim_out = 1;

    double operator()(const Eigen::VectorXd& x) const
    {
        Eigen::VectorXd opt(2);
        opt << 0.5, 0.5;

        return (x - opt).squaredNorm();
    }

    Eigen::MatrixXd solutions() const
    {
        Eigen::MatrixXd sols(1, 2);
        sols << 0.5, 0.5;
        return sols;
    }
};

struct Ellipsoid {
    static constexpr size_t dim_in = 2;
    static constexpr size_t dim_out = 1;

    double operator()(const Eigen::VectorXd& x) const
    {
        Eigen::VectorXd opt(2);
        opt << 0.5, 0.5;
        Eigen::VectorXd z = t_osz(x - opt);
        double r = 0;
        for (size_t i = 0; i < dim_in; ++i)
            r += std::pow(10, ((double)i) / (dim_in - 1.0)) * z(i) * z(i) + 1;
        return r;
    }

    Eigen::MatrixXd solutions() const
    {
        Eigen::MatrixXd sols(1, 2);
        sols << 0.5, 0.5;
        return sols;
    }
};

struct Rastrigin {
    static constexpr size_t dim_in = 4;
    static constexpr size_t dim_out = 1;

    double operator()(const Eigen::VectorXd& x) const
    {
        double f = 10 * dim_in;
        for (size_t i = 0; i < dim_in; ++i)
            f += x(i) * x(i) - 10 * cos(2 * M_PI * x(i));
        return f;
    }

    Eigen::MatrixXd solutions() const
    {
        Eigen::MatrixXd sols(1, 4);
        for (size_t i = 0; i < 4; ++i)
            sols(0, i) = 0;
        return sols;
    }
};

// see : http://www.sfu.ca/~ssurjano/hart3.html
struct Hartman3 {
    static constexpr size_t dim_in = 3;
    static constexpr size_t dim_out = 1;

    double operator()(const Eigen::VectorXd& x) const
    {
        Eigen::MatrixXd a(4, 3);
        Eigen::MatrixXd p(4, 3);
        a << 3.0, 10, 30, 0.1, 10, 35, 3.0, 10, 30, 0.1, 10, 36;
        p << 0.3689, 0.1170, 0.2673, 0.4699, 0.4387, 0.7470, 0.1091, 0.8732, 0.5547,
            0.0382, 0.5743, 0.8828;
        Eigen::VectorXd alpha(4);
        alpha << 1.0, 1.2, 3.0, 3.2;

        double res = 0;
        for (int i = 0; i < 4; i++) {
            double s = 0.0f;
            for (size_t j = 0; j < 3; j++) {
                s += a(i, j) * (x(j) - p(i, j)) * (x(j) - p(i, j));
            }
            res += alpha(i) * exp(-s);
        }
        return -res;
    }

    Eigen::MatrixXd solutions() const
    {
        Eigen::MatrixXd sols(1, 3);
        sols << 0.114614, 0.555649, 0.852547;
        return sols;
    }
};

// see : http://www.sfu.ca/~ssurjano/hart6.html
struct Hartman6 {
    static constexpr size_t dim_in = 6;
    static constexpr size_t dim_out = 1;

    double operator()(const Eigen::VectorXd& x) const
    {
        Eigen::MatrixXd a(4, 6);
        Eigen::MatrixXd p(4, 6);
        a << 10, 3, 17, 3.5, 1.7, 8, 0.05, 10, 17, 0.1, 8, 14, 3, 3.5, 1.7, 10, 17,
            8, 17, 8, 0.05, 10, 0.1, 14;
        p << 0.1312, 0.1696, 0.5569, 0.0124, 0.8283, 0.5886, 0.2329, 0.4135, 0.8307,
            0.3736, 0.1004, 0.9991, 0.2348, 0.1451, 0.3522, 0.2883, 0.3047, 0.665,
            0.4047, 0.8828, 0.8732, 0.5743, 0.1091, 0.0381;

        Eigen::VectorXd alpha(4);
        alpha << 1.0, 1.2, 3.0, 3.2;

        double res = 0;
        for (int i = 0; i < 4; i++) {
            double s = 0.0f;
            for (size_t j = 0; j < 6; j++) {
                s += a(i, j) * sqr(x(j) - p(i, j));
            }
            res += alpha(i) * exp(-s);
        }
        return -res;
    }

    Eigen::MatrixXd solutions() const
    {
        Eigen::MatrixXd sols(1, 6);
        sols << 0.20169, 0.150011, 0.476874, 0.275332, 0.311652, 0.6573;
        return sols;
    }
};

// see : http://www.sfu.ca/~ssurjano/goldpr.html
// (with ln, as suggested in Jones et al.)
struct GoldenPrice {
    static constexpr size_t dim_in = 2;
    static constexpr size_t dim_out = 1;

    double operator()(const Eigen::VectorXd& xx) const
    {
        Eigen::VectorXd x = (4.0 * xx);
        x(0) -= 2.0;
        x(1) -= 2.0;
        double r = (1 + (x(0) + x(1) + 1) * (x(0) + x(1) + 1) * (19 - 14 * x(0) + 3 * x(0) * x(0) - 14 * x(1) + 6 * x(0) * x(1) + 3 * x(1) * x(1))) * (30 + (2 * x(0) - 3 * x(1)) * (2 * x(0) - 3 * x(1)) * (18 - 32 * x(0) + 12 * x(0) * x(0) + 48 * x(1) - 36 * x(0) * x(1) + 27 * x(1) * x(1)));

        return log(r) - 5;
    }

    Eigen::MatrixXd solutions() const
    {
        Eigen::MatrixXd sols(1, 2);
        sols << 0.5, 0.25;
        return sols;
    }
};

struct BraninNormalized {
    static constexpr size_t dim_in = 2;
    static constexpr size_t dim_out = 1;

    double operator()(const Eigen::VectorXd& x) const
    {
        double a = x(0) * 15 - 5;
        double b = x(1) * 15;
        return sqr(b - (5.1 / (4 * sqr(M_PI))) * sqr(a) + 5 * a / M_PI - 6) + 10 * (1 - 1 / (8 * M_PI)) * cos(a) + 10;
    }

    Eigen::MatrixXd solutions() const
    {
        Eigen::MatrixXd sols(3, 2);
        sols << 0.1238938, 0.818333,
            0.5427728, 0.151667,
            0.961652, 0.1650;
        return sols;
    }
};

struct SixHumpCamel {
    static constexpr size_t dim_in = 2;
    static constexpr size_t dim_out = 1;
    double operator()(const Eigen::VectorXd& x) const
    {
        double x1_2 = x(0) * x(0);
        double x2_2 = x(1) * x(1);

        double tmp1 = (4 - 2.1 * x1_2 + (x1_2 * x1_2) / 3) * x1_2;
        double tmp2 = x(0) * x(1);
        double tmp3 = (-4 + 4 * x2_2) * x2_2;
        return tmp1 + tmp2 + tmp3;
    }

    Eigen::MatrixXd solutions() const
    {
        Eigen::MatrixXd sols(2, 2);
        sols << 0.0898, -0.7126,
            -0.0898, 0.7126;
        return sols;
    }
};

template <typename Function>
class Benchmark {
public:
    static constexpr size_t dim_in = Function::dim_in;
    static constexpr size_t dim_out = Function::dim_out;

    Eigen::VectorXd operator()(const Eigen::VectorXd& x) const
    {
        Eigen::VectorXd res(1);
        res(0) = -f(x);
        return res;
    }

    double accuracy(Eigen::VectorXd obs)
    {
        double x = obs[0];
        Eigen::MatrixXd sols = f.solutions();
        double diff = std::abs(x + f(sols.row(0)));
        double min_diff = diff;

        for (int i = 1; i < sols.rows(); i++) {
            diff = std::abs(x + f(sols.row(i)));
            if (diff < min_diff)
                min_diff = diff;
        }

        return min_diff;
    }

    Function f;
};
