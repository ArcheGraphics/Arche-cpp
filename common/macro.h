#pragma once

// From: https://github.com/Erlkoenig90/map-macro
#define VOX_MACRO_EVAL0(...) __VA_ARGS__
#define VOX_MACRO_EVAL1(...) VOX_MACRO_EVAL0(VOX_MACRO_EVAL0(VOX_MACRO_EVAL0(__VA_ARGS__)))
#define VOX_MACRO_EVAL2(...) VOX_MACRO_EVAL1(VOX_MACRO_EVAL1(VOX_MACRO_EVAL1(__VA_ARGS__)))
#define VOX_MACRO_EVAL3(...) VOX_MACRO_EVAL2(VOX_MACRO_EVAL2(VOX_MACRO_EVAL2(__VA_ARGS__)))
#define VOX_MACRO_EVAL4(...) VOX_MACRO_EVAL3(VOX_MACRO_EVAL3(VOX_MACRO_EVAL3(__VA_ARGS__)))
#define VOX_MACRO_EVAL5(...) VOX_MACRO_EVAL4(VOX_MACRO_EVAL4(VOX_MACRO_EVAL4(__VA_ARGS__)))
#define VOX_MACRO_EVAL(...) VOX_MACRO_EVAL5(__VA_ARGS__)

#define VOX_MACRO_EMPTY()
#define VOX_MACRO_DEFER(id) id VOX_MACRO_EMPTY()

// macro reverse
#define VOX_REVERSE_END(...)
#define VOX_REVERSE_OUT

#define VOX_REVERSE_GET_END2() 0, VOX_REVERSE_END
#define VOX_REVERSE_GET_END1(...) VOX_REVERSE_GET_END2
#define VOX_REVERSE_GET_END(...) VOX_REVERSE_GET_END1
#define VOX_REVERSE_NEXT0(test, next, ...) next VOX_REVERSE_OUT
#define VOX_REVERSE_NEXT1(test, next)    \
    VOX_MACRO_DEFER(VOX_REVERSE_NEXT0) \
    (test, next, 0)
#define VOX_REVERSE_NEXT(test, next) VOX_REVERSE_NEXT1(VOX_REVERSE_GET_END test, next)

#define VOX_REVERSE0(x, peek, ...)                            \
    VOX_MACRO_DEFER(VOX_REVERSE_NEXT(peek, VOX_REVERSE1)) \
    (peek, __VA_ARGS__) x,
#define VOX_REVERSE1(x, peek, ...)                            \
    VOX_MACRO_DEFER(VOX_REVERSE_NEXT(peek, VOX_REVERSE0)) \
    (peek, __VA_ARGS__) x,
#define VOX_REVERSE2(x, peek, ...)                            \
    VOX_MACRO_DEFER(VOX_REVERSE_NEXT(peek, VOX_REVERSE1)) \
    (peek, __VA_ARGS__) x
#define VOX_REVERSE(...) VOX_MACRO_EVAL(VOX_REVERSE2(__VA_ARGS__, ()()(), ()()(), ()()(), 0))

// macro map
#define VOX_MAP_END(...)
#define VOX_MAP_OUT

#define VOX_MAP_GET_END2() 0, VOX_MAP_END
#define VOX_MAP_GET_END1(...) VOX_MAP_GET_END2
#define VOX_MAP_GET_END(...) VOX_MAP_GET_END1
#define VOX_MAP_NEXT0(test, next, ...) next VOX_MAP_OUT
#define VOX_MAP_NEXT1(test, next)    \
    VOX_MACRO_DEFER(VOX_MAP_NEXT0) \
    (test, next, 0)
#define VOX_MAP_NEXT(test, next) VOX_MAP_NEXT1(VOX_MAP_GET_END test, next)

#define VOX_MAP0(f, x, peek, ...) f(x) VOX_MACRO_DEFER(VOX_MAP_NEXT(peek, VOX_MAP1))(f, peek, __VA_ARGS__)
#define VOX_MAP1(f, x, peek, ...) f(x) VOX_MACRO_DEFER(VOX_MAP_NEXT(peek, VOX_MAP0))(f, peek, __VA_ARGS__)

#define VOX_MAP_LIST0(f, x, peek, ...) , f(x) VOX_MACRO_DEFER(VOX_MAP_NEXT(peek, VOX_MAP_LIST1))(f, peek, __VA_ARGS__)
#define VOX_MAP_LIST1(f, x, peek, ...) , f(x) VOX_MACRO_DEFER(VOX_MAP_NEXT(peek, VOX_MAP_LIST0))(f, peek, __VA_ARGS__)
#define VOX_MAP_LIST2(f, x, peek, ...) f(x) VOX_MACRO_DEFER(VOX_MAP_NEXT(peek, VOX_MAP_LIST1))(f, peek, __VA_ARGS__)

// Applies the function macro `f` to each of the remaining parameters.
#define VOX_MAP(f, ...) VOX_MACRO_EVAL(VOX_MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

// Applies the function macro `f` to each of the remaining parameters and inserts commas between the results.
#define VOX_MAP_LIST(f, ...) VOX_MACRO_EVAL(VOX_MAP_LIST2(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

// other useful list operations...
#define VOX_TAIL_IMPL(x, ...) __VA_ARGS__
#define VOX_HEAD_IMPL(x, ...) x
#define VOX_HEAD(...) VOX_HEAD_IMPL(__VA_ARGS__)
#define VOX_TAIL(...) VOX_TAIL_IMPL(__VA_ARGS__)
#define VOX_LAST(...) VOX_HEAD(VOX_REVERSE(__VA_ARGS__))
#define VOX_POP_LAST(...) VOX_REVERSE(VOX_TAIL(VOX_REVERSE(__VA_ARGS__)))

// inc & dec
#define VOX_INC_0() 1
#define VOX_INC_1() 2
#define VOX_INC_2() 3
#define VOX_INC_3() 4
#define VOX_INC_4() 5
#define VOX_INC_5() 6
#define VOX_INC_6() 7
#define VOX_INC_7() 8
#define VOX_INC_8() 9
#define VOX_INC_9() 10
#define VOX_INC_10() 11
#define VOX_INC_11() 12
#define VOX_INC_12() 13
#define VOX_INC_13() 14
#define VOX_INC_14() 15
#define VOX_INC_15() 16
#define VOX_INC_16() 17
#define VOX_INC_17() 18
#define VOX_INC_18() 19
#define VOX_INC_19() 20
#define VOX_INC_20() 21
#define VOX_INC_21() 22
#define VOX_INC_22() 23
#define VOX_INC_23() 24
#define VOX_INC_24() 25
#define VOX_INC_25() 26
#define VOX_INC_26() 27
#define VOX_INC_27() 28
#define VOX_INC_28() 29
#define VOX_INC_29() 30
#define VOX_INC_30() 31
#define VOX_INC_31() 32
#define VOX_INC_32() 33
#define VOX_INC_33() 34
#define VOX_INC_34() 35
#define VOX_INC_35() 36
#define VOX_INC_36() 37
#define VOX_INC_37() 38
#define VOX_INC_38() 39
#define VOX_INC_39() 40
#define VOX_INC_40() 41
#define VOX_INC_41() 42
#define VOX_INC_42() 43
#define VOX_INC_43() 44
#define VOX_INC_44() 45
#define VOX_INC_45() 46
#define VOX_INC_46() 47
#define VOX_INC_47() 48
#define VOX_INC_48() 49
#define VOX_INC_49() 50
#define VOX_INC_50() 51
#define VOX_INC_51() 52
#define VOX_INC_52() 53
#define VOX_INC_53() 54
#define VOX_INC_54() 55
#define VOX_INC_55() 56
#define VOX_INC_56() 57
#define VOX_INC_57() 58
#define VOX_INC_58() 59
#define VOX_INC_59() 60
#define VOX_INC_60() 61
#define VOX_INC_61() 62
#define VOX_INC_62() 63
#define VOX_INC_63() 64
#define VOX_INC_IMPL(x) VOX_INC_##x()
#define VOX_INC(x) VOX_INC_IMPL(x)

#define VOX_DEC_1() 0
#define VOX_DEC_2() 1
#define VOX_DEC_3() 2
#define VOX_DEC_4() 3
#define VOX_DEC_5() 4
#define VOX_DEC_6() 5
#define VOX_DEC_7() 6
#define VOX_DEC_8() 7
#define VOX_DEC_9() 8
#define VOX_DEC_10() 9
#define VOX_DEC_11() 10
#define VOX_DEC_12() 11
#define VOX_DEC_13() 12
#define VOX_DEC_14() 13
#define VOX_DEC_15() 14
#define VOX_DEC_16() 15
#define VOX_DEC_17() 16
#define VOX_DEC_18() 17
#define VOX_DEC_19() 18
#define VOX_DEC_20() 19
#define VOX_DEC_21() 20
#define VOX_DEC_22() 21
#define VOX_DEC_23() 22
#define VOX_DEC_24() 23
#define VOX_DEC_25() 24
#define VOX_DEC_26() 25
#define VOX_DEC_27() 26
#define VOX_DEC_28() 27
#define VOX_DEC_29() 28
#define VOX_DEC_30() 29
#define VOX_DEC_31() 30
#define VOX_DEC_32() 31
#define VOX_DEC_33() 32
#define VOX_DEC_34() 33
#define VOX_DEC_35() 34
#define VOX_DEC_36() 35
#define VOX_DEC_37() 36
#define VOX_DEC_38() 37
#define VOX_DEC_39() 38
#define VOX_DEC_40() 39
#define VOX_DEC_41() 40
#define VOX_DEC_42() 41
#define VOX_DEC_43() 42
#define VOX_DEC_44() 43
#define VOX_DEC_45() 44
#define VOX_DEC_46() 45
#define VOX_DEC_47() 46
#define VOX_DEC_48() 47
#define VOX_DEC_49() 48
#define VOX_DEC_50() 49
#define VOX_DEC_51() 50
#define VOX_DEC_52() 51
#define VOX_DEC_53() 52
#define VOX_DEC_54() 53
#define VOX_DEC_55() 54
#define VOX_DEC_56() 55
#define VOX_DEC_57() 56
#define VOX_DEC_58() 57
#define VOX_DEC_59() 58
#define VOX_DEC_60() 59
#define VOX_DEC_61() 60
#define VOX_DEC_62() 61
#define VOX_DEC_63() 62
#define VOX_DEC_64() 63
#define VOX_DEC_IMPL(x) VOX_DEC_##x()
#define VOX_DEC(x) VOX_DEC_IMPL(x)

#define VOX_RANGE_GEN_1() 0
#define VOX_RANGE_GEN_2() 0, 1
#define VOX_RANGE_GEN_3() 0, 1, 2
#define VOX_RANGE_GEN_4() 0, 1, 2, 3
#define VOX_RANGE_GEN_5() 0, 1, 2, 3, 4
#define VOX_RANGE_GEN_6() 0, 1, 2, 3, 4, 5
#define VOX_RANGE_GEN_7() 0, 1, 2, 3, 4, 5, 6
#define VOX_RANGE_GEN_8() 0, 1, 2, 3, 4, 5, 6, 7
#define VOX_RANGE_GEN_9() 0, 1, 2, 3, 4, 5, 6, 7, 8
#define VOX_RANGE_GEN_10() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
#define VOX_RANGE_GEN_11() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
#define VOX_RANGE_GEN_12() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
#define VOX_RANGE_GEN_13() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
#define VOX_RANGE_GEN_14() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13
#define VOX_RANGE_GEN_15() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
#define VOX_RANGE_GEN_16() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
#define VOX_RANGE_GEN_17() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
#define VOX_RANGE_GEN_18() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
#define VOX_RANGE_GEN_19() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18
#define VOX_RANGE_GEN_20() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
#define VOX_RANGE_GEN_21() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20
#define VOX_RANGE_GEN_22() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21
#define VOX_RANGE_GEN_23() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22
#define VOX_RANGE_GEN_24() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
#define VOX_RANGE_GEN_25() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24
#define VOX_RANGE_GEN_26() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25
#define VOX_RANGE_GEN_27() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26
#define VOX_RANGE_GEN_28() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27
#define VOX_RANGE_GEN_29() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28
#define VOX_RANGE_GEN_30() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29
#define VOX_RANGE_GEN_31() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30
#define VOX_RANGE_GEN_32() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
#define VOX_RANGE_GEN_33() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32
#define VOX_RANGE_GEN_34() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33
#define VOX_RANGE_GEN_35() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34
#define VOX_RANGE_GEN_36() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
#define VOX_RANGE_GEN_37() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36
#define VOX_RANGE_GEN_38() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37
#define VOX_RANGE_GEN_39() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38
#define VOX_RANGE_GEN_40() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39
#define VOX_RANGE_GEN_41() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40
#define VOX_RANGE_GEN_42() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41
#define VOX_RANGE_GEN_43() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42
#define VOX_RANGE_GEN_44() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43
#define VOX_RANGE_GEN_45() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44
#define VOX_RANGE_GEN_46() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45
#define VOX_RANGE_GEN_47() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46
#define VOX_RANGE_GEN_48() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47
#define VOX_RANGE_GEN_49() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48
#define VOX_RANGE_GEN_50() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49
#define VOX_RANGE_GEN_51() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50
#define VOX_RANGE_GEN_52() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
#define VOX_RANGE_GEN_53() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52
#define VOX_RANGE_GEN_54() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53
#define VOX_RANGE_GEN_55() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54
#define VOX_RANGE_GEN_56() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55
#define VOX_RANGE_GEN_57() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56
#define VOX_RANGE_GEN_58() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57
#define VOX_RANGE_GEN_59() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58
#define VOX_RANGE_GEN_60() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59
#define VOX_RANGE_GEN_61() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60
#define VOX_RANGE_GEN_62() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61
#define VOX_RANGE_GEN_63() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62
#define VOX_RANGE_GEN_64() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
#define VOX_RANGE_GEN(N) VOX_RANGE_GEN_##N()
#define VOX_RANGE(N) VOX_RANGE_GEN(N)

#define VOX_STRINGIFY_IMPL(x) #x
#define VOX_STRINGIFY(x) VOX_STRINGIFY_IMPL(x)

#define VOX_AT_IMPL0(x, ...) x
#define VOX_AT_IMPL1(x, ...) VOX_AT_IMPL0(__VA_ARGS__)
#define VOX_AT_IMPL2(x, ...) VOX_AT_IMPL1(__VA_ARGS__)
#define VOX_AT_IMPL3(x, ...) VOX_AT_IMPL2(__VA_ARGS__)
#define VOX_AT_IMPL4(x, ...) VOX_AT_IMPL3(__VA_ARGS__)
#define VOX_AT_IMPL5(x, ...) VOX_AT_IMPL4(__VA_ARGS__)
#define VOX_AT_IMPL6(x, ...) VOX_AT_IMPL5(__VA_ARGS__)
#define VOX_AT_IMPL7(x, ...) VOX_AT_IMPL6(__VA_ARGS__)
#define VOX_AT_IMPL8(x, ...) VOX_AT_IMPL7(__VA_ARGS__)
#define VOX_AT_IMPL9(x, ...) VOX_AT_IMPL8(__VA_ARGS__)
#define VOX_AT_IMPL10(x, ...) VOX_AT_IMPL9(__VA_ARGS__)
#define VOX_AT_IMPL11(x, ...) VOX_AT_IMPL10(__VA_ARGS__)
#define VOX_AT_IMPL12(x, ...) VOX_AT_IMPL11(__VA_ARGS__)
#define VOX_AT_IMPL13(x, ...) VOX_AT_IMPL12(__VA_ARGS__)
#define VOX_AT_IMPL14(x, ...) VOX_AT_IMPL13(__VA_ARGS__)
#define VOX_AT_IMPL15(x, ...) VOX_AT_IMPL14(__VA_ARGS__)
#define VOX_AT_IMPL16(x, ...) VOX_AT_IMPL15(__VA_ARGS__)
#define VOX_AT_IMPL17(x, ...) VOX_AT_IMPL16(__VA_ARGS__)
#define VOX_AT_IMPL18(x, ...) VOX_AT_IMPL17(__VA_ARGS__)
#define VOX_AT_IMPL19(x, ...) VOX_AT_IMPL18(__VA_ARGS__)
#define VOX_AT_IMPL20(x, ...) VOX_AT_IMPL19(__VA_ARGS__)
#define VOX_AT_IMPL21(x, ...) VOX_AT_IMPL20(__VA_ARGS__)
#define VOX_AT_IMPL22(x, ...) VOX_AT_IMPL21(__VA_ARGS__)
#define VOX_AT_IMPL23(x, ...) VOX_AT_IMPL22(__VA_ARGS__)
#define VOX_AT_IMPL24(x, ...) VOX_AT_IMPL23(__VA_ARGS__)
#define VOX_AT_IMPL25(x, ...) VOX_AT_IMPL24(__VA_ARGS__)
#define VOX_AT_IMPL26(x, ...) VOX_AT_IMPL25(__VA_ARGS__)
#define VOX_AT_IMPL27(x, ...) VOX_AT_IMPL26(__VA_ARGS__)
#define VOX_AT_IMPL28(x, ...) VOX_AT_IMPL27(__VA_ARGS__)
#define VOX_AT_IMPL29(x, ...) VOX_AT_IMPL28(__VA_ARGS__)
#define VOX_AT_IMPL30(x, ...) VOX_AT_IMPL29(__VA_ARGS__)
#define VOX_AT_IMPL31(x, ...) VOX_AT_IMPL30(__VA_ARGS__)
#define VOX_AT_IMPL32(x, ...) VOX_AT_IMPL31(__VA_ARGS__)
#define VOX_AT_IMPL33(x, ...) VOX_AT_IMPL32(__VA_ARGS__)
#define VOX_AT_IMPL34(x, ...) VOX_AT_IMPL33(__VA_ARGS__)
#define VOX_AT_IMPL35(x, ...) VOX_AT_IMPL34(__VA_ARGS__)
#define VOX_AT_IMPL36(x, ...) VOX_AT_IMPL35(__VA_ARGS__)
#define VOX_AT_IMPL37(x, ...) VOX_AT_IMPL36(__VA_ARGS__)
#define VOX_AT_IMPL38(x, ...) VOX_AT_IMPL37(__VA_ARGS__)
#define VOX_AT_IMPL39(x, ...) VOX_AT_IMPL38(__VA_ARGS__)
#define VOX_AT_IMPL40(x, ...) VOX_AT_IMPL39(__VA_ARGS__)
#define VOX_AT_IMPL41(x, ...) VOX_AT_IMPL40(__VA_ARGS__)
#define VOX_AT_IMPL42(x, ...) VOX_AT_IMPL41(__VA_ARGS__)
#define VOX_AT_IMPL43(x, ...) VOX_AT_IMPL42(__VA_ARGS__)
#define VOX_AT_IMPL44(x, ...) VOX_AT_IMPL43(__VA_ARGS__)
#define VOX_AT_IMPL45(x, ...) VOX_AT_IMPL44(__VA_ARGS__)
#define VOX_AT_IMPL46(x, ...) VOX_AT_IMPL45(__VA_ARGS__)
#define VOX_AT_IMPL47(x, ...) VOX_AT_IMPL46(__VA_ARGS__)
#define VOX_AT_IMPL48(x, ...) VOX_AT_IMPL47(__VA_ARGS__)
#define VOX_AT_IMPL49(x, ...) VOX_AT_IMPL48(__VA_ARGS__)
#define VOX_AT_IMPL50(x, ...) VOX_AT_IMPL49(__VA_ARGS__)
#define VOX_AT_IMPL51(x, ...) VOX_AT_IMPL50(__VA_ARGS__)
#define VOX_AT_IMPL52(x, ...) VOX_AT_IMPL51(__VA_ARGS__)
#define VOX_AT_IMPL53(x, ...) VOX_AT_IMPL52(__VA_ARGS__)
#define VOX_AT_IMPL54(x, ...) VOX_AT_IMPL53(__VA_ARGS__)
#define VOX_AT_IMPL55(x, ...) VOX_AT_IMPL54(__VA_ARGS__)
#define VOX_AT_IMPL56(x, ...) VOX_AT_IMPL55(__VA_ARGS__)
#define VOX_AT_IMPL57(x, ...) VOX_AT_IMPL56(__VA_ARGS__)
#define VOX_AT_IMPL58(x, ...) VOX_AT_IMPL57(__VA_ARGS__)
#define VOX_AT_IMPL59(x, ...) VOX_AT_IMPL58(__VA_ARGS__)
#define VOX_AT_IMPL60(x, ...) VOX_AT_IMPL59(__VA_ARGS__)
#define VOX_AT_IMPL61(x, ...) VOX_AT_IMPL60(__VA_ARGS__)
#define VOX_AT_IMPL62(x, ...) VOX_AT_IMPL61(__VA_ARGS__)
#define VOX_AT_IMPL63(x, ...) VOX_AT_IMPL62(__VA_ARGS__)
#define VOX_AT(index, ...) VOX_AT_IMPL##index __VA_ARGS__

