#pragma once
#include <memory>
#include <vector>
#include <map>
class VtSystem;
typedef std::shared_ptr<VtSystem> SharedSystem;
typedef std::vector<SharedSystem> SharedSystemVec;
typedef std::map<int, SharedSystem> SharedSystemMap;