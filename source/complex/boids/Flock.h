#pragma once

#include "Unit.h"

#include <UnigineVector.h>

#include <atomic>
#include <memory>

class Flock
{
public:
	Flock();
	Flock(Flock&& other);

	void addUnit(Unit unit);

	void update();

	UNIGINE_INLINE size_t getSize() const { return units.size(); };

	UNIGINE_INLINE Unigine::Vector<Unit> &getUnits() { return units; }

	UNIGINE_INLINE void setCohesionCoefficient(float coef) { cohesion_coefficient = coef; }
	UNIGINE_INLINE float getCohesionCoefficient() const { return cohesion_coefficient; }

	UNIGINE_INLINE void setAlignmentCoefficient(float coef) { alignment_coefficient = coef; }
	UNIGINE_INLINE float getAlignmentCoefficient() const { return alignment_coefficient; }

	UNIGINE_INLINE void setSeparationCoefficient(float coef) { separation_coefficient = coef; }
	UNIGINE_INLINE float getSeparationCoefficient() const { return separation_coefficient; }

	UNIGINE_INLINE void setTargetChaseValue(float coef) { target_value = coef; }
	UNIGINE_INLINE float getTargetChaseValue() const { return target_value; }

	UNIGINE_INLINE const Unigine::NodePtr &getTarget() const { return flock_target_node; }
	UNIGINE_INLINE void setTarget(const Unigine::NodePtr &target) { flock_target_node = target; }

	UNIGINE_INLINE void setUseBound(bool use) { use_bound = use; }
	UNIGINE_INLINE bool getUseBound() const { return use_bound; }

private:
	class FlockUpdater final : Unigine::CPUShader
	{
	public:
		explicit FlockUpdater(Flock *flock);

		~FlockUpdater() override;

		void process(int thread_num, int num_threads) override;

		void run();

	private:
		Flock *flock{nullptr};
		std::atomic<int> counter{0};

	};

private:
	Unigine::Vector<Unit> units;

	float cohesion_coefficient{1.f};
	float alignment_coefficient{1.f};
	float separation_coefficient{1.f};

	Unigine::NodePtr flock_target_node{};
	float target_value{1.f};

	bool use_bound{false};

	std::unique_ptr<FlockUpdater> updater;
};
