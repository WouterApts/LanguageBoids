#include "gtest/gtest.h"
#include <Boid.h>
#include <iostream>
#include <Eigen/Dense>

class BoidFixture : public ::testing::Test {

protected:
    virtual void SetUp() {
        Eigen::Vector2f pos(1.0, 2.0);
        Eigen::Vector2f vel(3.0, 4.0);
        Eigen::Vector2f acc(5.0, 6.0);
        Eigen::VectorXi language(50);
        language << 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1;
        float language_influence = 0.75;
        float perception_radius = 10.0;
        float avoidance_radius = 5.0;
        float max_speed = 20.0;

        // Create the first Boid instance
        auto boid1 = std::make_shared<Boid>(pos, vel, acc, language, language_influence, perception_radius, avoidance_radius, max_speed);
        boids.push_back(boid1);
        for (int i = 0; i < 50; ++i) {
            auto boid = std::make_shared<Boid>(pos, vel, acc, language, language_influence, perception_radius, avoidance_radius, max_speed);
            boids.push_back(boid);
        }
        // Modify parameters for the second Boid instance
        language << 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0; // Change language for the second Boid
        Eigen::Vector2f pos2(2.0, 3.0); // Change position for the second Boid

        // Create the second Boid instance
        auto boid2 = std::make_shared<Boid>(pos2, vel, acc, language, language_influence, perception_radius, avoidance_radius, max_speed);
        boids.push_back(boid2);

        // Assign a Boid instance to test_boid pointer
        language << 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
        test_boid = new Boid(pos, vel, acc, language, language_influence, perception_radius, avoidance_radius, max_speed);

        Eigen::MatrixXf positions_matrix(2, boids.size());
        Eigen::MatrixXf velocities_matrix(2, boids.size());
        for(size_t i = 0; i < boids.size(); ++i) {
            positions_matrix.col(i) = boids[i]->pos;
            velocities_matrix.col(i) = boids[i]->vel;
        }

        Eigen::VectorXf similarities_vector = test_boid->CalcLanguageSimilarities(boids);
        this->positions = positions_matrix;
        this->velocities = velocities_matrix;
        this->similarities = similarities_vector;
    }

    std::vector<std::shared_ptr<Boid>> boids;
    std::vector<std::shared_ptr<Obstacle>> obstacles;
    Boid* test_boid{};
    Eigen::MatrixXf positions;
    Eigen::MatrixXf velocities;
    Eigen::VectorXf similarities;
};

TEST_F(BoidFixture, SimilarityTestEigen) {
    for (int i = 0; i < 10000; ++i) {
        Eigen::VectorXf v = test_boid->CalcLanguageSimilarities(boids);
    }
}

TEST_F(BoidFixture, AccelerationTest) {
    for (int i = 0; i < 10000; ++i) {
        Eigen::Vector2f acc = test_boid->CalcAcceleration(boids, obstacles, TODO);
    }
}

TEST_F(BoidFixture, CoherenceAlignmentTest) {
    for (int i = 0; i < 10000; ++i) {
        Eigen::Vector2f acc = test_boid->CalcCoherenceAlignmentAcceleration(positions, velocities,
                                                                            similarities);
    }
}




