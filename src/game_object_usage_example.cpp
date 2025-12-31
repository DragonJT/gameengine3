#include "game_object.hpp";


struct HealthTag{};
struct PosXTag{};
struct PosYTag{};

using Health = dod::Field<HealthTag, int>;
using PosX   = dod::Field<PosXTag, float>;
using PosY   = dod::Field<PosYTag, float>;

//using Creature   = ObjectType<Health, PosX, PosY>;
//using Projectile = ObjectType<PosX, PosY>;
using Creature2 = dod::ObjectType<Health, PosX, PosY>;


static_assert(GAME_OBJECT_V2_HPP_VERSION == 12345);


int main() {
  Creature2 c;
  static_assert(c.sanity_marker == 123);
  c.reserve(1024);
  c.emplace(100, 10.0f, 20.0f);   // Health, PosX, PosY (in template order)

  auto hx = c.span<Health>();
  auto x  = c.span<PosX>();
  auto y  = c.span<PosY>();

  for (std::size_t i = 0; i < c.size(); ++i) {
    hx[i] -= 1;
    x[i]  += 0.1f;
    y[i]  += 0.2f;
  }
}