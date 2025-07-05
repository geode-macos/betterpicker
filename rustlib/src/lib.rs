use palette::{FromColor, Hsl, Hsv, Okhsl, Okhsv, Srgb};

use crate::ffi::ColorTriple;

#[cxx::bridge]
mod ffi {

    #[derive(Debug)]
    struct ColorTriple {
        x: f32,
        y: f32,
        z: f32,
    }

    extern "Rust" {
        fn hsv_to_rgb(h: f32, s: f32, v: f32) -> ColorTriple;
        fn rgb_to_hsv(r: f32, g: f32, b: f32) -> ColorTriple;
        fn hsl_to_rgb(h: f32, s: f32, v: f32) -> ColorTriple;
        fn rgb_to_hsl(r: f32, g: f32, b: f32) -> ColorTriple;
        fn okhsv_to_rgb(h: f32, s: f32, v: f32) -> ColorTriple;
        fn rgb_to_okhsv(r: f32, g: f32, b: f32) -> ColorTriple;
        fn okhsl_to_rgb(h: f32, s: f32, v: f32) -> ColorTriple;
        fn rgb_to_okhsl(r: f32, g: f32, b: f32) -> ColorTriple;
    }
}

fn hsv_to_rgb(h: f32, s: f32, v: f32) -> ColorTriple {
    let hsv = Hsv::new_srgb(h * 360.0, s, v);
    let rgb = Srgb::from_color(hsv.into_format());
    ColorTriple {
        x: rgb.red,
        y: rgb.green,
        z: rgb.blue,
    }
}
fn rgb_to_hsv(r: f32, g: f32, b: f32) -> ColorTriple {
    let rgb = Srgb::new(r, g, b);
    let hsv = Hsv::from_color(rgb.into_format());
    ColorTriple {
        x: hsv.hue.into_inner() / 360.0,
        y: hsv.saturation,
        z: hsv.value,
    }
}

fn hsl_to_rgb(h: f32, s: f32, v: f32) -> ColorTriple {
    let hsv = Hsl::new_srgb(h * 360.0, s, v);
    let rgb = Srgb::from_color(hsv.into_format());
    ColorTriple {
        x: rgb.red,
        y: rgb.green,
        z: rgb.blue,
    }
}
fn rgb_to_hsl(r: f32, g: f32, b: f32) -> ColorTriple {
    let rgb = Srgb::new(r, g, b);
    let hsv = Hsl::from_color(rgb.into_format());
    ColorTriple {
        x: hsv.hue.into_inner() / 360.0,
        y: hsv.saturation,
        z: hsv.lightness,
    }
}

fn okhsv_to_rgb(h: f32, s: f32, v: f32) -> ColorTriple {
    let hsv = Okhsv::new(h * 360.0, s, v);
    let rgb = Srgb::from_color(hsv.into_format());
    ColorTriple {
        x: rgb.red,
        y: rgb.green,
        z: rgb.blue,
    }
}
fn rgb_to_okhsv(r: f32, g: f32, b: f32) -> ColorTriple {
    let rgb = Srgb::new(r, g, b);
    let hsv = Okhsv::from_color(rgb.into_format());
    ColorTriple {
        x: hsv.hue.into_inner() / 360.0,
        y: hsv.saturation,
        z: hsv.value,
    }
}

fn okhsl_to_rgb(h: f32, s: f32, v: f32) -> ColorTriple {
    let hsv = Okhsl::new(h * 360.0, s, v);
    let rgb = Srgb::from_color(hsv.into_format());
    ColorTriple {
        x: rgb.red,
        y: rgb.green,
        z: rgb.blue,
    }
}
fn rgb_to_okhsl(r: f32, g: f32, b: f32) -> ColorTriple {
    let rgb = Srgb::new(r, g, b);
    let hsv = Okhsl::from_color(rgb.into_format());
    ColorTriple {
        x: hsv.hue.into_inner() / 360.0,
        y: hsv.saturation,
        z: hsv.lightness,
    }
}
