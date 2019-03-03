#pragma once
#include <memory>
#include <vector>
class VtSystem;
typedef std::shared_ptr<VtSystem> SharedSystem;
typedef std::vector<SharedSystem> SharedSystemVec;