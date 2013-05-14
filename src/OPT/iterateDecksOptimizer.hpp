#ifndef ITERATEDECKS_OPT_ITERATEDECKSOPTIMIZER_HPP
    #define ITERATEDECKS_OPT_ITERATEDECKSOPTIMIZER_HPP

    class Optimizer {

        public:
            virtual DeckTemplate optimizeOnce(DeckTemplate & const initial) = 0;

    };

    class PraetorianOptimizer : Optimizer {

        private:
            std::set<DeckTemplate> mutate(DeckTemplate & const initial);

            void simpleCrop(std::set<DeckTemplate> & decks, double factor, unsigned int numberOfIterations);

        public:
            virtual DeckTemplate optimizeOnce(DeckTemplate & const initial);
    }

#end
