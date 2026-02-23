#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <chrono>

class AsianPricer {

    private :

    int T = 0; // date de maturité (en année(s))
    int N = 0; // nombre de dates d'observations
    int M = 0; // nombre de trajectoire 
    double sigma = 0.0;
    double strike = 0.0;
    double taux_r = 0.0;
    std::string type = "";
    double prix_sj_initial = 0.0;
    std::vector<double> payoff_vector;
    double estimation_valeur = 0.0;

    public :

    template<typename T, typename V>
    auto Payoff(T x, V y) {
        auto payoff = std::max(x, y);
        return payoff;
    }

    double générateur_loi_normale() {
        static thread_local std::mt19937 gen(std::random_device{}());
        std::normal_distribution<double> dist(0.0, 1.0);
        return dist(gen);
    }

    void user_info() {
        std::cout << "------------Start Asian Pricer------------";
        std::cout << "\nDonner le type d'option (call ou put) : ";
        std::cin >> type;
        std::cout << "\nDonner la date de maturite T : ";
        std::cin >> T;
        std::cout << "\nDonner le strike de l'option : ";
        std::cin >> strike;
        std::cout << "\nDonner le prix du sous-jacent a t = 0 : ";
        std::cin >> prix_sj_initial;
        std::cout << "\nDonner la volatilite de l'option : ";
        std::cin >> sigma;
        std::cout << "\nDonner le taux r : ";
        std::cin >> taux_r;
        std::cout << "\nDonner le nombre de trajectoires : ";
        std::cin >> M;
        std::cout << "\nDonner le nombre de dates d'observations : ";
        std::cin >> N;
    }

    double calculate_price() {
        double dt = T / static_cast<double>(N);
        double z = 0.0;
        double somme = 0.0;

        for (int j = 0; j < M; j++) {
            double prix_sj_local = prix_sj_initial;
            somme = 0.0;
            for (int i = 0; i < N; i++) {
                z = générateur_loi_normale();
                prix_sj_local *= exp((taux_r - 0.5 * pow(sigma, 2)) * dt + sigma * sqrt(dt) * z);
                somme += prix_sj_local;
            }
            double moyenne_sj = somme / static_cast<double>(N);
            if (type == "call") {
                payoff_vector.push_back(Payoff(moyenne_sj - strike, 0.0));
            }
            else {
                payoff_vector.push_back(Payoff(strike - moyenne_sj, 0.0));
            }
            
        }
        somme = 0.0;
        for (int i = 0; i < M; i++) {
            somme += payoff_vector[i];
        }
        estimation_valeur = somme / static_cast<double>(M);
        estimation_valeur *= exp(-taux_r * T);
        return estimation_valeur;
    }

    void user_finish(const double& resultat) {
        std::cout << "------------End Asian Pricer------------";
        std::cout << "\nEstimation de la valeur de l'option : " << resultat << std::endl;
    }

};

int main() {
    
    AsianPricer asianpricer;
    asianpricer.user_info();
    auto start = std::chrono::high_resolution_clock::now(); // début du chrono
    double resultat_asianpricer = asianpricer.calculate_price();
    auto finish = std::chrono::high_resolution_clock::now(); // début du chrono
    auto duration = finish - start;
    asianpricer.user_finish(resultat_asianpricer);
    std::cout << "Temps de calcul : " << duration.count() << " ms";

    return 0;
}