#pragma once

#include <vector>
#include <optional>

#include <Eigen/Dense>

class PolyMath
{
public:
    static std::vector<Eigen::Vector3f> BaseWindingForPlane(Eigen::Vector3f n, float d, float maxrange=8192);
    static bool PlaneCrosses(std::vector<Eigen::Vector3f> points, Eigen::Vector3f n, float d);
    static std::vector<Eigen::Vector3f> ClipToPlane(std::vector<Eigen::Vector3f> points, Eigen::Vector3f n, float d, bool front);
    static Eigen::Vector3f FindCenter(std::vector<Eigen::Vector3f> points);
    static Eigen::Vector3f FindNormal(std::vector<Eigen::Vector3f> points);
    static std::optional<Eigen::Vector3f> SegmentIntersects(std::vector<Eigen::Vector3f> points, Eigen::Vector3f a, Eigen::Vector3f b);
    static Eigen::Matrix3f PlaneProjection(Eigen::Vector3f n);
    static std::optional<Eigen::Vector3f> SegmentPlane(Eigen::Vector3f n, float d, Eigen::Vector3f a, Eigen::Vector3f b);
    static bool PointIn2d(std::vector<Eigen::Vector2f> points, Eigen::Vector2f p);
};