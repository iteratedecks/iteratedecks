#include "cliSimulate.hpp"
#include <cfloat>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <ios>

#include "../EXE/simulate.hpp"

namespace EvaluateDecks {
    namespace CLI {

        #define assertDE(a,b) assert(a-DBL_EPSILON < b && b < a+DBL_EPSILON)
        #define assertDEE(a,b,c) assert(std::abs(a-b) <= c)

        /**
         * @see http://en.wikipedia.org/wiki/Binomial_coefficient
         */
        long double binomialCoefficient(unsigned int const n
                                       ,unsigned int k
                                       )
        {
            assert(k <= n);
            if (n == 0) {
                return 0;
            } else if (2*k > n) {
                return binomialCoefficient(n,n-k);
            } else if (k == 0) {
                return 1;
            } else if (k == 1) {
                return n;
            }
            long double result = 1;
            for(unsigned int i = 1; i <= k; i++) {
                unsigned long const nom = n-k+i;
                unsigned long const denom = i;
                result = result * nom / denom;
            }
            //std::clog << "BinCo(" << n << "," << k << ") = ";
            //std::clog << result << std::endl;
            return result;
        }

        /**
         * x = B(X = k | p,n )
         */
        long double Bin(double const p
                       ,unsigned int const k
                       ,unsigned int const n
                       )
        {
            assert(0 <= p);
            assert(p <= 1);
            assert(k <= n);
            long double const q(1-p);
            long double const binCo(binomialCoefficient(n,k));
            assert(binomialCoefficient(12,6) == 924);
            assert(binomialCoefficient(11,5) == 462);
            if(0 < k && k < n) {
                long double const _bc1 = binomialCoefficient(n-1,k);
                long double const _bc2 = binomialCoefficient(n-1,k-1);
                long double const _bc = _bc1 + _bc2;
                //std::clog << binCo << " " << _bc << "  " << (binCo/_bc) << std::endl;
                assert(fabs(binCo / _bc - 1) <= 1e-8);
            }
            long double const pow1 (powl(p,k));
            long double const pow2 (powl(q,n-k));
            assert(binCo >= 0);
            assert(!std::isinf(binCo));
            assert(pow1 >= 0);
            assert(pow1 <= 1);
            assert(pow2 >= 0);
            assert(pow2 <= 1);
            long double x = binCo * pow1 * pow2;
            if(x != x) {
                std::cerr << "ouch!" << std::endl;
            }
            if(x < 0) {
                std::clog << ">|" << x << "|<" << std::endl;
                // you would assume that the product of 3 non-negative numbers is non-negative. You would be wrong.
                x = 0;
            }
            assert(x>=0); //< that really should not happen anymore... but floating math is a tricky beast
            assert(x<=1);
            if(p >= 0.1) {
                //std::clog << "Bin(X = " << k << "|" << p << "," << n << ") = " << x << std::endl;
            }
            return x;
        }

        /**
         * x = B(X <= k | p,n)
         * @return x
         */
        long double cumBin(double const p
                          ,unsigned int const k
                          ,unsigned int const n
                          )
        {
            assertDE(Bin(0.125,12,12),1.4551915228366851807E-11);
            assertDE(Bin(0.125,6,12),1.5819048858247697353E-3);

            long double sumL = 0.0d;
            long double sumU = 0.0d;
            for(unsigned int i = 0; i <= n; i++) {
                if (i <= k) {
                    sumL += Bin(p,i,n);
                } else {
                    sumU += Bin(p,i,n);
                }
            }
            if(p >= 0.01) {
                //std::clog << "Bin(X <= " << k << "|" << p << "," << n << ") = " << sumL << std::endl;
            }
            if(1 < sumL && sumL <= 1 + 1e-6) {
                sumL = 1;
            }
            if(1 < sumU && sumU <= 1 + 1e-6) {
                sumU = 1;
            }
            assert(sumL>=0-DBL_EPSILON);
            assert(sumL<=1+DBL_EPSILON);
            assert(sumU>=0-DBL_EPSILON);
            assert(sumU<=1+DBL_EPSILON);
            long double sum = sumL + sumU;
            //std::clog << "Total sum of probabilites for Bin( true |" <<  p << "," << n << ") = " << sum << std::endl;
            //assertDE(sum,1);

            // we normalize this result by sum, which should be 1, but probably is not exactly one
            return sumL / sum;
        }

        /**
         * x = B(X <= k | p,n )
         * where B is the Binomial distribution.
         * @return p
         */
        double cumBinInv(double const x
                        ,unsigned int const k
                        ,unsigned int const n
                        )
        {
            assert(k <= n);
            assert(0 <= x);
            assert(x <= 1);

            assertDEE(cumBin(0.5,9,12),0.9807128906,1e-8);

            // use search...
            double pl(0);
            double pu(1);

            double const eps(DBL_EPSILON);
            while (pl + eps < pu) {
                double const pm = (pl+pu)/2;
                double const testX(cumBin(pm,k,n));

                if(x < testX) {
                    // test value is larged as target value
                    // cumBin is monotonic decreasing in p
                    // we want to decrease test value, thus we need to increase argument
                    pl = pm;
                } else if (x > testX) {
                    pu = pm;
                } else {
                    pl = pm;
                    pu = pm;
                }

                //std::clog << "approximating cumBinInv(" << x << ", " << k << ", " << n << ") ";
                //std::clog << "pm=" << pm << " ";
                //std::clog << "testX=" << testX << " ";
                //std::clog << "delta=" << (x-testX) << std::endl;
            }
            double const pm = (pl+pu)/2;
            assertDEE(cumBin(pm,k,n),x,1e-12);
            return pm;
        }


        // http://home.online.no/~pjacklam/notes/invnorm/
        long double normInv(double const p)
        {
            double const a1 =  -39.69683028665376;
            double const a2 =  220.9460984245205;
            double const a3 = -275.9285104469687;
            double const a4 =  138.3577518672690;
            double const a5 =  -30.66479806614716;
            double const a6 =    2.506628277459239;

            double const b1 =  -54.47609879822406;
            double const b2 =  161.5858368580409;
            double const b3 = -155.6989798598866;
            double const b4 =   66.80131188771972;
            double const b5 =  -13.28068155288572;

            double const c1 =   -0.007784894002430293;
            double const c2 =   -0.3223964580411365;
            double const c3 =   -2.400758277161838;
            double const c4 =   -2.549732539343734;
            double const c5 =    4.374664141464968;
            double const c6 =    2.938163982698783;

            double const d1 =    0.007784695709041462;
            double const d2 =    0.3224671290700398;
            double const d3 =    2.445134137142996;
            double const d4 =    3.754408661907416;

            double const pLow =  0.02425;
            double const pHigh = 1 - pLow;

            long double x;
            long double q, r, u, e;

            assert(p > 0);
            if (0 < p && p < pLow) {
                q = sqrt(-2*log(p));
                x = (((((c1*q+c2)*q+c3)*q+c4)*q+c5)*q+c6) / ((((d1*q+d2)*q+d3)*q+d4)*q+1);
            } else if (pLow <= p && p <= pHigh) {
                q = p - 0.5;
                r = q*q;
                x = (((((a1*r+a2)*r+a3)*r+a4)*r+a5)*r+a6)*q /(((((b1*r+b2)*r+b3)*r+b4)*r+b5)*r+1);
            } else if (pHigh < p && p < 1) {
                q = sqrt(-2*log(1-p));
                x = -(((((c1*q+c2)*q+c3)*q+c4)*q+c5)*q+c6) / ((((d1*q+d2)*q+d3)*q+d4)*q+1);
            }
            assert(p<1);

            if (0 < p && p < 1) {
                e = 0.5 * erfc(-x/sqrt(2)) - p;
                u = e * sqrt(2*M_PI) * exp(x*x/2);
                x = x - u/(1 + x*u/2);
            }
            return x;
        }

        double lowerBound(unsigned int const k  //< wins
                         ,unsigned int const n  //< games
                         ,double const gamma    //< confidence level
                         )
        {
            assert(k <= n);
            assert(0 <= gamma);
            assert(gamma <= 1);
            double const alpha(1-gamma);
            if (k == 0) {
                return 0.0d;
            } else if (n <= 1000) {
                // unfortunately this will become numerically unstable for large n
                return cumBinInv(1-alpha/2,k-1,n);
            } else {
                double const estimator ((double)k / (double)n);
                double const c = normInv(1-alpha/2);
                double const squareroot = sqrt(estimator * (1-estimator) / n);
                return estimator - c * squareroot;
            }
        }

        double upperBound(unsigned int const k  //< successfull results
                         ,unsigned int const n  //< total tries
                         ,double const gamma    //< confidence level
                         )
        {
            assert(k <= n);
            assert(0 <= gamma);
            assert(gamma <= 1);
            double const alpha(1-gamma);
            if (k == n) {
                return 1.0d;
            } else if (n <= 1000) {
                // unfortunately this will become numerically unstable for large n
                return cumBinInv(alpha/2,k,n);
            } else {
                double const estimator ((double)k / (double)n);
                double const c = normInv(1-alpha/2);
                double const squareroot = sqrt(estimator * (1-estimator) / n);
                return estimator + c * squareroot;
            }
        }


        RESULTS simulate(ActiveDeck const & deck1
                        ,ActiveDeck const & deck2
                        ,DeckLogger & attackLogger
                        ,DeckLogger & defenseLogger
                        ,SimulationLogger & simulationLogger
                        ,unsigned int const & numberOfIterations
                        )
        {
            // Here comes the actual simulations
            RESULTS r;
            for (UINT k=0;k<numberOfIterations;k++)	{
                ActiveDeck X(deck1);
                X.logger = &attackLogger;
                ActiveDeck Y(deck2);
                Y.logger = &defenseLogger;

                simulationLogger.simulationStart(k);
                Simulate(X,Y,r,&simulationLogger);
                simulationLogger.simulationEnd(k);
            }

            // Print result
            std::cout << "Games won: " << std::setw(11) << r.Win << "/" << std::setw(11) << r.Games << " ";
            std::cout.flush();
            double const winRate = (double)r.Win / (double)r.Games; // estimator
            std::cout << "estimator=" << std::setiosflags(std::ios::fixed) << std::setprecision(4) << winRate << " ";
            std::cout.flush();
            double const gamma(0.95); // confidence level
            double const lBound(lowerBound(r.Win,r.Games,gamma));
            double const uBound(upperBound(r.Win,r.Games,gamma));
            std::cout << "confidence [" << lBound << ";" << uBound << "]";
            assert(0 <= lBound);
            assert(lBound <= winRate);
            assert(winRate <= uBound);
            assert(uBound <= 1);
            std::cout << "; ";
            double const averageNetPoints ((double)r.Points / (double)r.Games);
            std::cout << "ANP=" << averageNetPoints;
            std::cout << std::endl;
            return r;
        }

    } // namespace CLI
} // namespace EvaluateDecks
