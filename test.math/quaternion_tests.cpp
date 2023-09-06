//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unit_tests_utils.h"
#include "math/quaternion.h"

using namespace vox;

TEST(Quaternion, Constructors) {
    {
        QuaternionD q;

        EXPECT_DOUBLE_EQ(1.0, q.w);
        EXPECT_DOUBLE_EQ(0.0, q.x);
        EXPECT_DOUBLE_EQ(0.0, q.y);
        EXPECT_DOUBLE_EQ(0.0, q.z);
    }
    {
        QuaternionD q(2, 3, 4, 1);

        EXPECT_DOUBLE_EQ(1.0, q.w);
        EXPECT_DOUBLE_EQ(2.0, q.x);
        EXPECT_DOUBLE_EQ(3.0, q.y);
        EXPECT_DOUBLE_EQ(4.0, q.z);
    }
    {
        QuaternionD q(QuaternionD(2, 3, 4, 1));

        EXPECT_DOUBLE_EQ(1.0, q.w);
        EXPECT_DOUBLE_EQ(2.0, q.x);
        EXPECT_DOUBLE_EQ(3.0, q.y);
        EXPECT_DOUBLE_EQ(4.0, q.z);
    }
    {
        QuaternionD q({2, 3, 4, 1});

        EXPECT_DOUBLE_EQ(1.0, q.w);
        EXPECT_DOUBLE_EQ(2.0, q.x);
        EXPECT_DOUBLE_EQ(3.0, q.y);
        EXPECT_DOUBLE_EQ(4.0, q.z);
    }
    {
        // set with axis & angle
        Vector3D originalAxis = Vector3D(1, 3, 2).normalized();
        double originalAngle = 0.4;

        QuaternionD q(originalAxis, originalAngle);

        Vector3D axis = q.axis();
        double angle = q.angle();

        EXPECT_DOUBLE_EQ(originalAxis.x, axis.x);
        EXPECT_DOUBLE_EQ(originalAxis.y, axis.y);
        EXPECT_DOUBLE_EQ(originalAxis.z, axis.z);
        EXPECT_DOUBLE_EQ(originalAngle, angle);
    }

    {
        // set with from & to vectors (90 degrees)
        Vector3D from(1, 0, 0);
        Vector3D to(0, 0, 1);

        QuaternionD q(from, to);

        Vector3D axis = q.axis();
        double angle = q.angle();

        EXPECT_DOUBLE_EQ(0.0, axis.x);
        EXPECT_DOUBLE_EQ(-1.0, axis.y);
        EXPECT_DOUBLE_EQ(0.0, axis.z);
        EXPECT_DOUBLE_EQ(pi<double>() / 2.0, angle);
    }
    {
        Vector3D rotationBasis0(1, 0, 0);
        Vector3D rotationBasis1(0, 0, 1);
        Vector3D rotationBasis2(0, -1, 0);

        QuaternionD q(rotationBasis0, rotationBasis1, rotationBasis2);

        EXPECT_DOUBLE_EQ(std::sqrt(2.0) / 2.0, q.w);
        EXPECT_DOUBLE_EQ(std::sqrt(2.0) / 2.0, q.x);
        EXPECT_DOUBLE_EQ(0.0, q.y);
        EXPECT_DOUBLE_EQ(0.0, q.z);
    }
}

TEST(Quaternion, BasicSetters) {
    {
        QuaternionD q;
        q.set(QuaternionD(2, 3, 4, 1));

        EXPECT_DOUBLE_EQ(1.0, q.w);
        EXPECT_DOUBLE_EQ(2.0, q.x);
        EXPECT_DOUBLE_EQ(3.0, q.y);
        EXPECT_DOUBLE_EQ(4.0, q.z);
    }
    {
        QuaternionD q;
        q.set(2, 3, 4, 1);

        EXPECT_DOUBLE_EQ(1.0, q.w);
        EXPECT_DOUBLE_EQ(2.0, q.x);
        EXPECT_DOUBLE_EQ(3.0, q.y);
        EXPECT_DOUBLE_EQ(4.0, q.z);
    }
    {
        QuaternionD q;
        q.set({2, 3, 4, 1});

        EXPECT_DOUBLE_EQ(1.0, q.w);
        EXPECT_DOUBLE_EQ(2.0, q.x);
        EXPECT_DOUBLE_EQ(3.0, q.y);
        EXPECT_DOUBLE_EQ(4.0, q.z);
    }
    {
        // set with axis & angle
        Vector3D originalAxis = Vector3D(1, 3, 2).normalized();
        double originalAngle = 0.4;

        QuaternionD q;
        q.set(originalAxis, originalAngle);

        Vector3D axis = q.axis();
        double angle = q.angle();

        EXPECT_DOUBLE_EQ(originalAxis.x, axis.x);
        EXPECT_DOUBLE_EQ(originalAxis.y, axis.y);
        EXPECT_DOUBLE_EQ(originalAxis.z, axis.z);
        EXPECT_DOUBLE_EQ(originalAngle, angle);
    }

    {
        // set with from & to vectors (90 degrees)
        Vector3D from(1, 0, 0);
        Vector3D to(0, 0, 1);

        QuaternionD q;
        q.set(from, to);

        Vector3D axis = q.axis();
        double angle = q.angle();

        EXPECT_DOUBLE_EQ(0.0, axis.x);
        EXPECT_DOUBLE_EQ(-1.0, axis.y);
        EXPECT_DOUBLE_EQ(0.0, axis.z);
        EXPECT_DOUBLE_EQ(pi<double>() / 2.0, angle);
    }
    {
        Vector3D rotationBasis0(1, 0, 0);
        Vector3D rotationBasis1(0, 0, 1);
        Vector3D rotationBasis2(0, -1, 0);

        QuaternionD q;
        q.set(rotationBasis0, rotationBasis1, rotationBasis2);

        EXPECT_DOUBLE_EQ(std::sqrt(2.0) / 2.0, q.w);
        EXPECT_DOUBLE_EQ(std::sqrt(2.0) / 2.0, q.x);
        EXPECT_DOUBLE_EQ(0.0, q.y);
        EXPECT_DOUBLE_EQ(0.0, q.z);
    }
}

TEST(Quaternion, CastTo) {
    QuaternionD qd(2, 3, 4, 1);
    QuaternionF qf = qd.castTo<float>();

    EXPECT_FLOAT_EQ(2.f, qf.x);
    EXPECT_FLOAT_EQ(3.f, qf.y);
    EXPECT_FLOAT_EQ(4.f, qf.z);
    EXPECT_FLOAT_EQ(1.f, qf.w);
}

TEST(Quaternion, Normalized) {
    QuaternionD q(2, 3, 4, 1);
    QuaternionD qn = q.normalized();

    double denom = std::sqrt(30.0);
    EXPECT_DOUBLE_EQ(1.0 / denom, qn.w);
    EXPECT_DOUBLE_EQ(2.0 / denom, qn.x);
    EXPECT_DOUBLE_EQ(3.0 / denom, qn.y);
    EXPECT_DOUBLE_EQ(4.0 / denom, qn.z);
}

TEST(Quaternion, BinaryOperators) {
    QuaternionD q1(2, 3, 4, 1);
    QuaternionD q2(-2, -3, -4, 1);

    QuaternionD q3 = q1.mul(q2);

    EXPECT_DOUBLE_EQ(30.0, q3.w);
    EXPECT_DOUBLE_EQ(0.0, q3.x);
    EXPECT_DOUBLE_EQ(0.0, q3.y);
    EXPECT_DOUBLE_EQ(0.0, q3.z);

    q1.normalize();
    Vector3D v(7, 8, 9);
    Vector3D ans1 = q1.mul(v);

    Matrix3x3D m = q1.matrix3();
    Vector3D ans2 = m.mul(v);

    EXPECT_DOUBLE_EQ(ans2.x, ans1.x);
    EXPECT_DOUBLE_EQ(ans2.y, ans1.y);
    EXPECT_DOUBLE_EQ(ans2.z, ans1.z);

    q1.set(2, 3, 4, 1);
    q2.set(6, 7, 8, 5);
    EXPECT_DOUBLE_EQ(70.0, q1.dot(q2));

    q3 = q1.mul(q2);
    EXPECT_EQ(q3, q2.rmul(q1));
    q1.imul(q2);
    EXPECT_EQ(q3, q1);
}

TEST(Quaternion, Modifiers) {
    QuaternionD q(3, 2, 1, 4);
    q.setIdentity();

    EXPECT_DOUBLE_EQ(1.0, q.w);
    EXPECT_DOUBLE_EQ(0.0, q.x);
    EXPECT_DOUBLE_EQ(0.0, q.y);
    EXPECT_DOUBLE_EQ(0.0, q.z);

    q.set(3, 2, 1, 4);
    q.normalize();

    double denom = std::sqrt(30.0);
    EXPECT_DOUBLE_EQ(4.0 / denom, q.w);
    EXPECT_DOUBLE_EQ(3.0 / denom, q.x);
    EXPECT_DOUBLE_EQ(2.0 / denom, q.y);
    EXPECT_DOUBLE_EQ(1.0 / denom, q.z);

    Vector3D axis;
    double angle;
    q.getAxisAngle(&axis, &angle);
    q.rotate(1.0);
    double newAngle;
    q.getAxisAngle(&axis, &newAngle);

    EXPECT_DOUBLE_EQ(angle + 1.0, newAngle);
}

TEST(Quaternion, ComplexGetters) {
    QuaternionD q(2, 3, 4, 1);

    QuaternionD q2 = q.inverse();
    EXPECT_DOUBLE_EQ(1.0 / 30.0, q2.w);
    EXPECT_DOUBLE_EQ(-1.0 / 15.0, q2.x);
    EXPECT_DOUBLE_EQ(-1.0 / 10.0, q2.y);
    EXPECT_DOUBLE_EQ(-2.0 / 15.0, q2.z);

    q.set(0, 5, 2, 1);
    q.normalize();
    Matrix3x3D mat3 = q.matrix3();
    double solution3[9] = {-14.0 / 15.0, 2.0 / 15.0, -1.0 / 3.0, -2.0 / 15.0, 11.0 / 15.0,
                           2.0 / 3.0,    1.0 / 3.0,  2.0 / 3.0,  -2.0 / 3.0};

    for (int i = 0; i < 9; ++i) {
        EXPECT_DOUBLE_EQ(solution3[i], mat3[i]);
    }

    Matrix4x4D mat4 = q.matrix4();
    double solution4[16] = {-14.0 / 15.0, 2.0 / 15.0, -1.0 / 3.0, 0.0, -2.0 / 15.0, 11.0 / 15.0, 2.0 / 3.0, 0.0,
                            1.0 / 3.0,    2.0 / 3.0,  -2.0 / 3.0, 0.0, 0.0,         0.0,         0.0,       1.0};

    Vector3D axis;
    double angle;
    q.getAxisAngle(&axis, &angle);

    EXPECT_DOUBLE_EQ(0.0, axis.x);
    EXPECT_DOUBLE_EQ(5.0 / std::sqrt(29.0), axis.y);
    EXPECT_DOUBLE_EQ(2.0 / std::sqrt(29.0), axis.z);

    EXPECT_EQ(axis, q.axis());
    EXPECT_EQ(angle, q.angle());

    EXPECT_DOUBLE_EQ(2.0 * std::acos(1.0 / std::sqrt(30.0)), angle);

    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(solution4[i], mat4[i]);
    }

    q.set(2, 3, 4, 1);
    EXPECT_DOUBLE_EQ(std::sqrt(30.0), q.l2Norm());
}

TEST(Quaternion, SetterOperators) {
    QuaternionD q(2, 3, 4, 1);
    QuaternionD q2(6, 7, 8, 1);

    q2 = q;
    EXPECT_EQ(1.0, q2.w);
    EXPECT_EQ(2.0, q2.x);
    EXPECT_EQ(3.0, q2.y);
    EXPECT_EQ(4.0, q2.z);

    q2.set(6, 7, 8, 5);

    q *= q2;

    EXPECT_EQ(-60.0, q.w);
    EXPECT_EQ(12.0, q.x);
    EXPECT_EQ(30.0, q.y);
    EXPECT_EQ(24.0, q.z);
}

TEST(Quaternion, GetterOperators) {
    QuaternionD q(2, 3, 4, 1);

    EXPECT_EQ(1.0, q[0]);
    EXPECT_EQ(2.0, q[1]);
    EXPECT_EQ(3.0, q[2]);
    EXPECT_EQ(4.0, q[3]);

    QuaternionD q2(2, 3, 4, 1);
    EXPECT_TRUE(q == q2);

    q[0] = 5.0;
    q[1] = 6.0;
    q[2] = 7.0;
    q[3] = 8.0;

    EXPECT_EQ(5.0, q[0]);
    EXPECT_EQ(6.0, q[1]);
    EXPECT_EQ(7.0, q[2]);
    EXPECT_EQ(8.0, q[3]);

    EXPECT_TRUE(q != q2);
}

// MARK: -
TEST(Quaternion, add) {
    const auto a = QuaternionD(2, 3, 4, 1);
    const auto b = QuaternionD(-3, 5, 0, 2);
    const auto out = a + b;

    EXPECT_VECTOR4_EQ(out, QuaternionD(-1, 8, 4, 3));
}

TEST(Quaternion, multiply) {
    const auto a = QuaternionD(2, 3, 4, 1);
    const auto b = QuaternionD(-3, 5, 0, 2);
    const auto out = a * b;

    EXPECT_VECTOR4_EQ(out, QuaternionD(-19, -1, 27, -7));
}

TEST(Quaternion, conjugate) {
    const auto a = QuaternionD(2, 3, 4, 5);
    const auto out = a.conjugate();

    EXPECT_VECTOR4_EQ(out, QuaternionD(-2, -3, -4, 5));
}

TEST(Quaternion, dot) {
    const auto a = QuaternionD(2, 3, 1, 1);
    const auto b = QuaternionD(-4, 5, 1, 1);

    EXPECT_EQ(a.dot(b), 9);
}

TEST(Quaternion, rotationAxisAngle) {
    const auto a = Vector3D(3, 7, 5);
    const auto out = Quaternion(a, M_PI / 3);
    auto b = Vector3D();
    double rad = 0;
    out.getAxisAngle(&b, &rad);

    EXPECT_EQ(rad, M_PI / 3);
    EXPECT_VECTOR3_EQ(b.normalized(), a.normalized());
}

TEST(Quaternion, rotationEuler) {
    const auto out1 = QuaternionD::makeRotationEuler(0, M_PI / 3, M_PI / 2);
    const auto out2 = QuaternionD::makeRotationYawPitchRoll(0, M_PI / 3, M_PI / 2);

    const auto a = out1.toEuler();
    const auto b = out2.toYawPitchRoll();
    EXPECT_VECTOR3_EQ(a, Vector3D(0, M_PI / 3, M_PI / 2));
    EXPECT_VECTOR3_EQ(b, Vector3D(0, M_PI / 3, M_PI / 2));
}

TEST(Quaternion, rotationMatrix3x3) {
    const auto a1 = Matrix3x3D(1, 2, 3, 4, 5, 6, 7, 8, 9);
    const auto a2 = Matrix3x3D(1, 2, 3, 4, -5, 6, 7, 8, -9);
    const auto a3 = Matrix3x3D(1, 2, 3, 4, 5, 6, 7, 8, -9);
    const auto a4 = Matrix3x3D(-7, 2, 3, 4, -5, 6, 7, 8, 9);

    auto out = QuaternionD(a1);
    EXPECT_VECTOR4_EQ(out, QuaternionD(-0.25, 0.5, -0.25, 2));
    out = QuaternionD(a2);
    EXPECT_VECTOR4_EQ(out, QuaternionD(2, 0.75, 1.25, -0.25));
    out = QuaternionD(a3);
    EXPECT_VECTOR4_EQ(out, QuaternionD(0.8017837257372732, 1.8708286933869707, 1.8708286933869709, 0.5345224838248488));
    out = QuaternionD(a4);
    EXPECT_VECTOR4_EQ(out, QuaternionD(1.066003581778052, 1.4924050144892729, 2.345207879911715, -0.21320071635561041));
}

TEST(Quaternion, invert) {
    const auto a = QuaternionD(1, 1, 1, 0.5);
    const auto out = a.inverse();

    EXPECT_VECTOR4_EQ(out,
                      QuaternionD(-0.3076923076923077, -0.3076923076923077, -0.3076923076923077, 0.15384615384615385));
}

TEST(Quaternion, lerp) {
    const auto a = QuaternionD(0, 1, 2, 0);
    const auto b = QuaternionD(2, 2, 0, 0);
    const auto normal = QuaternionD(1, 1.5, 1, 0);
    const auto out = lerp(a, b, 0.5);

    EXPECT_VECTOR4_EQ(out, normal);
}

TEST(Quaternion, slerp) {
    const auto a = QuaternionD(1, 1, 1, 0.5);
    const auto b = QuaternionD(0.5, 0.5, 0.5, 0.5);
    const auto out = slerp(a, b, 0.5);

    EXPECT_VECTOR4_EQ(out, QuaternionD(0.75, 0.75, 0.75, 0.5));
}

TEST(Quaternion, normalize) {
    const auto a = QuaternionD(3, 4, 0, 0);
    const auto out = a.normalized();

    EXPECT_VECTOR4_EQ(out, QuaternionD(0.6, 0.8, 0, 0));
}

TEST(Quaternion, rotation) {
    auto out = QuaternionD::makeRotationX(1.5);
    EXPECT_VECTOR4_EQ(out, QuaternionD(0.6816387600233341, 0, 0, 0.7316888688738209));

    out = QuaternionD::makeRotationY(1.5);
    EXPECT_VECTOR4_EQ(out, QuaternionD(0, 0.6816387600233341, 0, 0.7316888688738209));

    out = QuaternionD::makeRotationZ(1.5);
    EXPECT_VECTOR4_EQ(out, QuaternionD(0, 0, 0.6816387600233341, 0.7316888688738209));
}

TEST(Quaternion, rotate) {
    const auto a = QuaternionD();
    auto b = QuaternionD();

    auto out = a.rotateX(1.5);
    b.rotateX(1.5);
    EXPECT_VECTOR4_EQ(out, QuaternionD(0.6816387600233341, 0, 0, 0.7316888688738209));
    EXPECT_VECTOR4_EQ(out, b);

    out = a.rotateY(1.5);
    b.set(0, 0, 0, 1);
    b.rotateY(1.5);
    EXPECT_VECTOR4_EQ(out, QuaternionD(0, 0.6816387600233341, 0, 0.7316888688738209));
    EXPECT_VECTOR4_EQ(out, b);

    out = a.rotateZ(1.5);
    b.set(0, 0, 0, 1);
    b.rotateZ(1.5);
    EXPECT_VECTOR4_EQ(out, QuaternionD(0, 0, 0.6816387600233341, 0.7316888688738209));
    EXPECT_VECTOR4_EQ(out, b);
}

TEST(Quaternion, rotatAxisAngle) {
    const auto a = Vector3D(0, 5, 0);
    const double b = 0.5 * M_PI;
    auto out = QuaternionD(0, 0, 0, 1);
    out.rotateAxisAngle(a, b);
    EXPECT_VECTOR4_EQ(out, QuaternionD(0, 0.7071067811865475, 0, 0.7071067811865476));
}

TEST(Quaternion, scale) {
    const auto a = QuaternionD(3, 4, 5, 0);
    const auto out = a * 3.0;
    EXPECT_VECTOR4_EQ(out, QuaternionD(9, 12, 15, 0));
}

TEST(Quaternion, toEuler) {
    const auto a = QuaternionD::makeRotationEuler(0, M_PI / 3.0, 0);
    const auto euler = a.toEuler();
    const auto ypr = a.toYawPitchRoll();
    EXPECT_VECTOR3_EQ(euler, Vector3D(0, M_PI / 3, 0));
    EXPECT_VECTOR3_EQ(ypr, Vector3D(M_PI / 3, 0, 0));
}

TEST(Quaternion, identity) {
    const auto a = QuaternionD::makeIdentity();

    EXPECT_VECTOR4_EQ(a, QuaternionD(0, 0, 0, 1));
}

TEST(Quaternion, length) {
    const auto a = QuaternionD(3, 4, 5, 0);
    EXPECT_EQ(a.length(), std::sqrt(50));
    EXPECT_EQ(a.lengthSquared(), 50);
}
