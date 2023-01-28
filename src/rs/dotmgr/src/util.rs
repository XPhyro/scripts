extern crate termion;

use die_exit::*;
use termion::color;

pub fn die(err: String) {
    return Err("error").die_with(|_error| {
        format!(
            "{}error{}: {}",
            color::Red.fg_str(),
            color::Reset.fg_str(),
            err
        )
    });
}
