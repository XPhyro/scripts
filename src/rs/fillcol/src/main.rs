#[macro_use]
extern crate fstrings;
extern crate termion;

use std::io;

use clap::Parser;
use crossterm::cursor;
use exitfailure::ExitFailure;
use termion::color;

/// Fill the columns of the terminal with the given messages.
#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Args {
    /// Set the color of the background.
    #[arg(short = 'b', long = "color", default_value = "clear")]
    bg_color: String,

    /// Do not clear the color after printing.
    #[arg(short = 'c', long = "no-clear")]
    no_clear: bool,

    /// Set the color of the message.
    #[arg(short = 'f', long = "color", default_value = "white")]
    fg_color: String,

    /// Column to start filling at.
    #[arg(required = true)]
    column: u16,

    /// Messages to print. If multiple are given, they are concatenated with a space.
    #[arg(required = true)]
    messages: Vec<String>,
}

fn main() -> Result<(), ExitFailure> {
    let args = Args::parse();

    let fg_color = match args.fg_color.as_str() {
        "black" => color::Black.fg_str(),
        "blue" => color::Blue.fg_str(),
        "clear" => color::Reset.fg_str(),
        "cyan" => color::Cyan.fg_str(),
        "green" => color::Green.fg_str(),
        "lightblack" => color::LightBlack.fg_str(),
        "lightblue" => color::LightBlue.fg_str(),
        "lightcyan" => color::LightCyan.fg_str(),
        "lightgreen" => color::LightGreen.fg_str(),
        "lightmagenta" => color::LightMagenta.fg_str(),
        "lightred" => color::LightRed.fg_str(),
        "lightwhite" => color::LightWhite.fg_str(),
        "lightyellow" => color::LightYellow.fg_str(),
        "magenta" => color::Magenta.fg_str(),
        "red" => color::Red.fg_str(),
        "white" => color::White.fg_str(),
        "yellow" => color::Yellow.fg_str(),
        _ => {
            return Err(io::Error::new(
                io::ErrorKind::InvalidInput,
                f!("Invalid color: {args.fg_color}"),
            )
            .into())
        }
    };

    let bg_color = match args.bg_color.as_str() {
        "black" => color::Black.bg_str(),
        "blue" => color::Blue.bg_str(),
        "clear" => color::Reset.bg_str(),
        "cyan" => color::Cyan.bg_str(),
        "green" => color::Green.bg_str(),
        "lightblack" => color::LightBlack.bg_str(),
        "lightblue" => color::LightBlue.bg_str(),
        "lightcyan" => color::LightCyan.bg_str(),
        "lightgreen" => color::LightGreen.bg_str(),
        "lightmagenta" => color::LightMagenta.bg_str(),
        "lightred" => color::LightRed.bg_str(),
        "lightwhite" => color::LightWhite.bg_str(),
        "lightyellow" => color::LightYellow.bg_str(),
        "magenta" => color::Magenta.bg_str(),
        "red" => color::Red.bg_str(),
        "white" => color::White.bg_str(),
        "yellow" => color::Yellow.bg_str(),
        _ => {
            return Err(io::Error::new(
                io::ErrorKind::InvalidInput,
                f!("Invalid color: {args.bg_color}"),
            )
            .into())
        }
    };

    let (term_width, term_height) = termion::terminal_size().unwrap();

    let mut col = args.column;
    let mut msg_len: u16 = 0;
    for msg in &args.messages {
        msg_len += msg.len() as u16 + 1;
    }
    msg_len -= 1;

    if col > term_width {
        col = term_width - msg_len + 1;
    } else if col < 1 {
        col = 1;
    }

    let cursor = cursor();

    cursor.save_position()?;

    for line in 1..term_height + 1 {
        print!(
            "{}{}{}",
            termion::cursor::Goto(col, line),
            fg_color,
            bg_color
        );
        let last = args.messages.len() - 1;
        for i in 0..args.messages.len() {
            print!("{}{}", args.messages[i], if i != last { " " } else { "" });
        }
        if !args.no_clear {
            print!("{}{}", color::Reset.fg_str(), color::Reset.bg_str());
        }
    }

    cursor.restore_position()?;

    Ok(())
}
