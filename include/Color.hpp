struct Vec3 {
    float x;
    float y;
    float z;
};

struct Vec2 {
    float x;
    float y;
};

Vec3 rgb_to_hsl(float r, float g, float b);
Vec3 hsl_to_rgb(float h, float s, float l);

Vec3 rgb_to_hsv(float r, float g, float b);
Vec3 hsv_to_rgb(float h, float s, float v);

Vec3 srgb_to_okhsl(float r, float g, float b);
Vec3 okhsl_to_srgb(float h, float s, float l);

Vec3 srgb_to_okhsv(float r, float g, float b);
Vec3 okhsv_to_srgb(float h, float s, float v);