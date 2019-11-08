#include "player/core.h"
#include "tests.h"

static const struct unittest *unittests[] = {
    &test_chmap,
    &test_gl_video,
    &test_json,
    &test_linked_list,
    NULL
};

bool run_tests(struct MPContext *mpctx)
{
    char *sel = mpctx->opts->test_mode;
    assert(sel && sel[0]);

    if (strcmp(sel, "help") == 0) {
        MP_INFO(mpctx, "Available tests:\n");
        for (int n = 0; unittests[n]; n++)
            MP_INFO(mpctx, "   %s\n", unittests[n]->name);
        MP_INFO(mpctx, "   all-simple\n");
        return true;
    }

    struct test_ctx ctx = {
        .global = mpctx->global,
        .log = mpctx->log,
    };

    int num_run = 0;

    for (int n = 0; unittests[n]; n++) {
        const struct unittest *t = unittests[n];

        // Exactly 1 entrypoint please.
        assert(MP_IS_POWER_OF_2(
            (t->run        ? (1 << 1) : 0)));

        bool run = false;
        run |= strcmp(sel, "all-simple") == 0 && !t->is_complex;
        run |= strcmp(sel, t->name);

        if (run) {
            if (t->run)
                t->run(&ctx);
            num_run++;
        }
    }

    MP_INFO(mpctx, "%d unittests successfully run.\n", num_run);

    return num_run > 0; // still error if none
}

#ifdef NDEBUG
static_assert(false, "don't define NDEBUG for tests");
#endif

void assert_int_equal_impl(const char *file, int line, int64_t a, int64_t b)
{
    if (a != b) {
        printf("%s:%d: %"PRId64" != %"PRId64"\n", file, line, a, b);
        abort();
    }
}

void assert_string_equal_impl(const char *file, int line,
                              const char *a, const char *b)
{
    if (strcmp(a, b) != 0) {
        printf("%s:%d: '%s' != '%s'\n", file, line, a, b);
        abort();
    }
}

void assert_float_equal_impl(const char *file, int line,
                              double a, double b, double tolerance)
{
    if (fabs(a - b) > tolerance) {
        printf("%s:%d: %f != %f\n", file, line, a, b);
        abort();
    }
}