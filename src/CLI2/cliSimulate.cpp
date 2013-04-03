#include "cliSimulate.hpp"
#include <cfloat>
#define _USE_MATH_DEFINES // Needed for VSC++ to pick up M_PI. Do not move this below erf.hpp
#include <cmath>

#include <cassert>
#include <iostream>
#include <iomanip>
#include <ios>
#include <limits>

#ifdef _WIN32
    #define __windows__
#endif
#ifdef _WIN64
    #define __windows__
#endif

#if defined(__windows__) && !defined(__MINGW32__)
    #include "boost/math/special_functions/erf.hpp"
    #define erfc boost::math::erfc

    #define isinf(x) (!_finite(x))
#else
    #define isinf(x) (std::isinf(x))
#endif

#include "../CORE/cards.hpp"
#include "../CORE/simulate.hpp"
//#include "../CORE/simulate.h"
#include "../CORE/assert.hpp"

#include "../CORE/activeDeck.hpp"

#include "../CORE/cardDB.hpp"

#include "../CORE/iterateDecksCore.hpp"
#include "SimulationTaskStruct.hpp"

namespace IterateDecks {
    namespace CLI {

        #define assertDE(a,b) assert(a-DBL_EPSILON < b && b < a+DBL_EPSILON)

        #define M_PI 3.14159265358979323846

        /**
         * @see http://en.wikipedia.org/wiki/Binomial_coefficient
         */
        unsigned long binomialCoefficient(unsigned int const n
                                         ,unsigned int k
                                         )
        {
            assert(n <= 34); // else we overflow
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
            unsigned long nomProd = 1;
            unsigned long denomProd = 1;
            for(unsigned int i = 1; i <= k; i++) {
                unsigned long const nom = n-k+i;
                unsigned long const denom = i;

                if (denomProd % nom == 0) {
                    denomProd /= nom;
                } else {
                    if (nomProd > std::numeric_limits<unsigned long>::max() / nom) {
                        std::cerr << "nom overflow for n=" << n << " nomProd="<< nomProd << " nom=" << nom << std::endl;
                        std::cerr << "max=" << std::numeric_limits<unsigned long>::max() << " max/nom=" << std::numeric_limits<unsigned long>::max() / nom << std::endl;
                        assert(false);
                    }
                    nomProd *= nom;
                }
                if (nomProd % denom == 0) {
                    nomProd /= denom;
                } else {
                    if(denomProd > std::numeric_limits<unsigned long>::max() / denom) {
                        std::cerr << "denom overflow for n=" << n << " denomProd="<< denomProd << " denom=" << denom << std::endl;
                        std::cerr << "max=" << std::numeric_limits<unsigned long>::max() << " max/denom=" << std::numeric_limits<unsigned long>::max() / denom << std::endl;
                        assert(false);
                    }
                    denomProd *= denom;
                }
            }
            assert(nomProd % denomProd == 0);
            unsigned long result = nomProd / denomProd;
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
            unsigned long const binCo(binomialCoefficient(n,k));
            //assert(binomialCoefficient(12,6) == 924);
            //assert(binomialCoefficient(11,5) == 462);
            if(0 < k && k < n) {
                unsigned long const bc1 = binomialCoefficient(n-1,k);
                unsigned long const bc2 = binomialCoefficient(n-1,k-1);
                unsigned long const bc = bc1 + bc2;
                //std::clog << binCo << " " << _bc << "  " << (binCo/_bc) << std::endl;
                assert(binCo == bc);
            }
            #if _BSD_SOURCE || _SVID_SOURCE || _XOPEN_SOURCE >= 600 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L
                long double const pow1 (powl(p,k));
                long double const pow2 (powl(q,n-k));
            #else
                long double const pow1 (pow(p,k));
                long double const pow2 (pow(q,n-k));
            #endif
            assert(binCo >= 0);
            assert(!isinf(binCo));
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
            //assertDE(Bin(0.125,12,12),1.4551915228366851807E-11);
            //assertDE(Bin(0.125,6,12),1.5819048858247697353E-3);

            long double sumL = 0.0;
            long double sumU = 0.0;
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

            //assertDEE(cumBin(0.5,9,12),0.9807128906,1e-8);

            // use search...
            double pl(0);
            double pu(1);

            double const eps(1e-4);
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
            assertDEE(cumBin(pm,k,n),x,1e-3);
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

        /**
         * Compute centered two sided confidence interval.
         */
        void wilson(unsigned int const k    //< wins
                   ,unsigned int const n    //< games
                   ,double const gamma      //< confidence
                   ,bool const twoSided
                   ,double & lower          //< output: lower bound
                   ,double & upper          //< output: upper bound
                   )
        {
            assertX(k <= n);
            assertX(0 <= gamma);
            assertX(gamma <= 1);
            double const alpha = 1-gamma;
            // wilson
            double const estimator ((double)k / (double)n);
            double const confArg = twoSided ? 1-alpha/2 : 1-alpha;
            double const c = normInv(confArg);
            assertX(!isinf(c));
            double const cSquare = c*c;
            double const factor1 = 1 / (1 + cSquare / n);
            double const summand2 = cSquare / 2 / n;
            assertX(!isinf(estimator * (1-estimator) / n));
            assertX(!isinf(cSquare));
            assertX(n>0);
            assertX(4*n*n > 0);
            assertX(!isinf(cSquare / (4*n*n)));
            double const radicand = estimator * (1-estimator) / n  + cSquare / (4*n*n);
            assertX(!isinf(c));
            assertX(!isinf(radicand));
            double const summand3 = c * sqrt(radicand);
            assertX(estimator >= 0);
            assertX(summand2 >= 0);
            assertX(!isinf(summand3));
            assertGE(estimator + summand2 + .001, summand3);
            double const factor2l = estimator + summand2 - summand3;
            double const factor2u = estimator + summand2 + summand3;
            assertX(factor1 + 0.000 >= 0);
            assertX(factor2l + 0.001 >= 0);
            assertX(factor2u + 0.001 >= 0);
            lower = factor1 * factor2l;
            upper = factor1 * factor2u;
        }

        void twoSidedBounds(unsigned int const k    //< wins
                   ,unsigned int const n    //< games
                   ,double const gamma      //< confidence level
                   ,double & lower          //< output: lower bound
                   ,double & upper          //< output: upper bound
                   )
        {
            double const alpha = 1 - gamma;
            if(n <= 34) {
                if (k == 0) {
                    lower = 0;
                } else {
                    double const alphaL = alpha/2;
                    lower = cumBinInv(1-alphaL,k-1,n);
                }
                if (k == n) {
                    upper = 1;
                } else {
                    double const alphaU = alpha/2;
                    upper = cumBinInv(alphaU,k,n);
                }
            } else {
                wilson(k,n,gamma,true,lower,upper);
            }
            assertLE(0 -.001,lower);
            assertLE(lower -.001,upper);
            assertLE(upper -.001,1.0);
        }


        double oneSidedLowerBound(unsigned int const k  //< wins
                                 ,unsigned int const n  //< games
                                 ,double const gamma    //< confidence level
                                 )
        {
            assert(k <= n);
            assert(0 <= gamma);
            assert(gamma <= 1);
            double const alpha(1-gamma);
            double result;
            if (k == 0) {
                return 0.0;
            } else if (n <= 34) {
                // unfortunately this will become numerically unstable for large n
                result = cumBinInv(1-alpha/2,k-1,n);
                assertX(result >= 0);
            } else {
                double upper;
                wilson(k,n,gamma,false,result,upper);
            }
            assertX(result >= 0);
            return result;
        }

        double oneSidedUpperBound(unsigned int const k  //< successfull results
                                 ,unsigned int const n  //< total tries
                                 ,double const gamma    //< confidence level
                                 )
        {
            assert(k <= n);
            assert(0 <= gamma);
            assert(gamma <= 1);
            double const alpha(1-gamma);
            if (k == n) {
                return 1.0;
            } else if (n <= 34) {
                // unfortunately this will become numerically unstable for large n
                return cumBinInv(alpha/2,k,n);
            } else {
                double lower;
                double upper;
                wilson(k,n,gamma,false,lower,upper);
                return upper;
            }
        }

        void checkIntervals(unsigned int const k1
                           ,unsigned int const n1
                           ,unsigned int const k2
                           ,unsigned int const n2
                           ,double confidence
                           ,std::string msg = std::string()
                           )
        {
            double l1, u1, l2, u2;
            twoSidedBounds(k1, n1, confidence, l1, u1);
            twoSidedBounds(k2, n2, confidence, l2, u2);
            std::stringstream ssMessage;
            ssMessage << msg;
            ssMessage << ", k1=" << k1;
            ssMessage << ", n1=" << n1;
            ssMessage << ", k2=" << k2;
            ssMessage << ", n2=" << n2;
            ssMessage << ": ";
            std::string const message = ssMessage.str();
            assertLEM(l1, u2, message);
            assertGEM(u1, l2, message);
        }


        RESULTS simulate(DeckArgument const & attackDeck
                        ,DeckArgument const & defenseDeck
                        ,DeckLogger * attackLogger
                        ,DeckLogger * defenseLogger
                        ,SimulationLogger * simulationLogger
                        ,unsigned int const & numberOfIterations
                        ,bool surge
                        ,BattleGroundEffect battleGroundEffect
                        ,CardDB const & cardDB
                        ,unsigned int seed
                        ,bool allowInvalidDecks
                        ,Logger * logger
                        )
        {
            // construct the decks
            // TODO own function for this.
            // ... first deck
            assertEQ(attackDeck.getType(), DeckArgument::HASH);
            // FIXME: should not pass c_str
            ActiveDeck deck1(attackDeck.getHash().c_str(), cardDB.GetPointer());
            deck1.SetOrderMatters(attackDeck.isOrdered());
            if(battleGroundEffect != BattleGroundEffect::normal) {
                deck1.SetQuestEffect(battleGroundEffect);
            }

            RESULTS r;
            switch(defenseDeck.getType()) {
                case DeckArgument::HASH: {
                    ActiveDeck deck2(defenseDeck.getHash().c_str(), cardDB.GetPointer());
                    deck2.SetOrderMatters(defenseDeck.isOrdered());
                    if(battleGroundEffect != BattleGroundEffect::normal) {
                        deck2.SetQuestEffect(battleGroundEffect);
                    }
                    r = simulateSimple(deck1,deck2,attackLogger,defenseLogger,simulationLogger, numberOfIterations, surge);
                } break;

                case DeckArgument::RAID_ID: {
                    r = simulateRaid(deck1, defenseDeck.getRaidId(), attackLogger, defenseLogger, simulationLogger, numberOfIterations);
                } break;

                case DeckArgument::QUEST_ID: {
                    r = simulateQuest(deck1, defenseDeck.getQuestId(), attackLogger, defenseLogger, simulationLogger, numberOfIterations);
                } break;

                case DeckArgument::MISSION_ID: {
                    ActiveDeck deck2 = cardDB.GetMissionDeck(defenseDeck.getMissionId());
                    if(battleGroundEffect != BattleGroundEffect::normal) {
                        deck2.SetQuestEffect(battleGroundEffect);
                    }
                    r = simulateSimple(deck1,deck2,attackLogger,defenseLogger,simulationLogger, numberOfIterations, surge);
                } break;
            }

            // Refactoring: Also use new simulator to check for same results.
            IterateDecksCore simulator;
            simulator.setLogger(logger);
            SimulationTaskStruct task;
            task.minimalNumberOfGames = numberOfIterations;
            task.surge = surge;
            task.delayFirstAttacker = false;
            task.battleGround = BattleGroundEffect::normal;
            task.achievementOptions = AchievementOptions();
            task.randomSeed = seed;
            task.attacker = attackDeck;
            task.defender = defenseDeck;
            task.allowInvalidDecks = allowInvalidDecks;

            SimulationTaskClass taskClass = task.toSimulationTaskClass(cardDB);
            Result r2 = simulator.simulate(taskClass);
            // Compare
            assertEQ(r.Games, r2.numberOfGames);
            assertEQ(r2.numberOfGames, r2.gamesWon + r2.gamesLost + r2.gamesStalled);
            checkIntervals(r.Win               , r.Games, r2.gamesWon    , r2.numberOfGames, 0.9, "win");
            checkIntervals(r.Loss              , r.Games, r2.gamesLost   , r2.numberOfGames, 0.9, "loss");
            checkIntervals(r.Games-r.Win-r.Loss, r.Games, r2.gamesStalled, r2.numberOfGames, 0.9, "draw");
            //std::clog << "P" << r.Points << ", "
            //          << "AP" << r.AutoPoints << ", "
            //          << "LP" << r.LPoints << ", "
            //          << "LAP" << r.LAutoPoints << std::endl;
            //std::clog << "AM" << r2.pointsAttacker << ", "
            //          << "AA" << r2.pointsAttackerAuto << ", "
            //          << "DM" << r2.pointsDefender << ", "
            //          << "DA" << r2.pointsDefenderAuto << std::endl;
            assertDEE(static_cast<double>(r.Points)      / static_cast<double>(r.Games), static_cast<double>(r2.pointsAttacker    ) / static_cast<double>(r2.numberOfGames), 2);
            assertDEE(static_cast<double>(r.AutoPoints)  / static_cast<double>(r.Games), static_cast<double>(r2.pointsAttackerAuto) / static_cast<double>(r2.numberOfGames), 2);
            // NETRAT decided to be lazy and not implement points for Quest and Raid, hence we cannot check them
            if(defenseDeck.getType() != DeckArgument::RAID_ID && defenseDeck.getType() != DeckArgument::QUEST_ID) {
                assertDEE(static_cast<double>(r.LPoints)     / static_cast<double>(r.Games), static_cast<double>(r2.pointsDefender    ) / static_cast<double>(r2.numberOfGames), 2);
                assertDEE(static_cast<double>(r.LAutoPoints) / static_cast<double>(r.Games), static_cast<double>(r2.pointsDefenderAuto) / static_cast<double>(r2.numberOfGames), 2);
            }
            return r;
        }


        RESULTS simulateSimple(ActiveDeck const & deck1
                        ,ActiveDeck const & deck2
                        ,DeckLogger * attackLogger
                        ,DeckLogger * defenseLogger
                        ,SimulationLogger * simulationLogger
                        ,unsigned int const & numberOfIterations
                        ,bool surge
                        )
        {
            // Here come the actual simulations
            RESULTS r;
            for (UINT k=0;k<numberOfIterations;k++)	{
                ActiveDeck X(deck1);
                X.logger = attackLogger;
                ActiveDeck Y(deck2);
                Y.logger = defenseLogger;

                LOG(simulationLogger,simulationStart(k));
                IterateDecks::Core::Simulate(X,Y,r,simulationLogger, NULL, NULL, surge);
                LOG(simulationLogger,simulationEnd(k));
            }
            return r;
        }

        RESULTS simulateRaid(ActiveDeck const & deck1
                            , unsigned int const & raidId
                            , DeckLogger * attackLogger
                            , DeckLogger * defenseLogger
                            , SimulationLogger * simulationLogger
                            , unsigned int const & numberOfIterations
                            )
        {
            // Here comes the actual simulations
            RESULTS r;
            for (UINT k=0;k<numberOfIterations;k++)    {
                ActiveDeck X(deck1);
                X.logger = attackLogger;

                LOG(simulationLogger,simulationStart(k));
                // TODO we should add support for a defense logger
                EvaluateRaidQuestOnce(X,r,0,0,(DWORD)raidId,0);
                LOG(simulationLogger,simulationEnd(k));
            }
            return r;
        }

        RESULTS simulateQuest(ActiveDeck const & deck1
                            , unsigned int const & questId
                            , DeckLogger * attackLogger
                            , DeckLogger * defenseLogger
                            , SimulationLogger * simulationLogger
                            , unsigned int const & numberOfIterations
                            )
        {
            // Here comes the actual simulations
            RESULTS r;
            for (UINT k=0;k<numberOfIterations;k++)    {
                ActiveDeck X(deck1);
                X.logger = attackLogger;

                LOG(simulationLogger,simulationStart(k));
                // TODO we should add support for a defense logger
                EvaluateRaidQuestOnce(X,r,0,0,0,(DWORD)questId);
                LOG(simulationLogger,simulationEnd(k));
            }
            return r;
        }

        void printResults(RESULTS r) {
            // Print result
            std::cout << "Games won:   " << std::setw(11) << r.Win << " /" << std::setw(11) << r.Games << " " << std::endl;
            std::cout << "Games lost:  " << std::setw(11) << r.Loss << " /" << std::setw(11) << r.Games << " " << std::endl;
            std::cout << "Games drawn: " << std::setw(11) << r.Games - r.Loss - r.Win << " /" << std::setw(11) << r.Games << " " << std::endl;

            if(r.Win > 0 && r.Win < r.Games) {
                double const winRate = (double)r.Win / (double)r.Games; // estimator
                std::cout << "estimator=" << std::setiosflags(std::ios::fixed) << std::setprecision(4) << winRate << " ";
                std::cout.flush();
                double const gamma(0.90); // confidence level
                double lBound, uBound;
                twoSidedBounds(r.Win, r.Games, gamma, lBound, uBound);
                std::cout << "confidence [" << lBound << ";" << uBound << "]";
                assertLE(0 -.001,lBound);
                assertLE(lBound -.001,winRate);
                assertLE(winRate -.001, uBound);
                assertLE(uBound -.001,1.0);
            } else {
                std::cout << "confidence skipped";
            }
            std::cout << "; ";
            double const averageNetPoints ((double)r.Points / (double)r.Games);
            std::cout << "ANP=" << averageNetPoints;
            std::cout << std::endl;
        }

    } // namespace CLI
} // namespace EvaluateDecks
