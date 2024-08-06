#[macro_use]
extern crate fstrings;
extern crate termion;

use std::io;

use clap::Parser;
use exitfailure::ExitFailure;
use isatty::stdin_isatty;
use termion::color;
use termion::{cursor::DetectCursorPos, raw::IntoRawMode};

/// Query information about the terminal or control it.
#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Args {
    /// Command to run.
    #[arg(required = true)]
    command: String,

    /// Subcommand to run.
    #[arg(required = true)]
    subcommand: String,

    /// Positional arguments for the subcommand.
    #[arg(required = false)]
    args: Vec<String>,
}

fn get_bg_ansi(color_str: String) -> Result<&'static str, ExitFailure> {
    return match color_str.as_str() {
        "black" => Ok(color::Black.bg_str()),
        "blue" => Ok(color::Blue.bg_str()),
        "clear" => Ok(color::Reset.bg_str()),
        "cyan" => Ok(color::Cyan.bg_str()),
        "green" => Ok(color::Green.bg_str()),
        "lightblack" => Ok(color::LightBlack.bg_str()),
        "lightblue" => Ok(color::LightBlue.bg_str()),
        "lightcyan" => Ok(color::LightCyan.bg_str()),
        "lightgreen" => Ok(color::LightGreen.bg_str()),
        "lightmagenta" => Ok(color::LightMagenta.bg_str()),
        "lightred" => Ok(color::LightRed.bg_str()),
        "lightwhite" => Ok(color::LightWhite.bg_str()),
        "lightyellow" => Ok(color::LightYellow.bg_str()),
        "magenta" => Ok(color::Magenta.bg_str()),
        "red" => Ok(color::Red.bg_str()),
        "white" => Ok(color::White.bg_str()),
        "yellow" => Ok(color::Yellow.bg_str()),
        _ => {
            return Err(io::Error::new(
                io::ErrorKind::InvalidInput,
                f!("Invalid color: {color_str}"),
            )
            .into())
        }
    };
}

fn get_fg_ansi(color_str: String) -> Result<&'static str, ExitFailure> {
    return match color_str.as_str() {
        "black" => Ok(color::Black.fg_str()),
        "blue" => Ok(color::Blue.fg_str()),
        "clear" => Ok(color::Reset.fg_str()),
        "cyan" => Ok(color::Cyan.fg_str()),
        "green" => Ok(color::Green.fg_str()),
        "lightblack" => Ok(color::LightBlack.fg_str()),
        "lightblue" => Ok(color::LightBlue.fg_str()),
        "lightcyan" => Ok(color::LightCyan.fg_str()),
        "lightgreen" => Ok(color::LightGreen.fg_str()),
        "lightmagenta" => Ok(color::LightMagenta.fg_str()),
        "lightred" => Ok(color::LightRed.fg_str()),
        "lightwhite" => Ok(color::LightWhite.fg_str()),
        "lightyellow" => Ok(color::LightYellow.fg_str()),
        "magenta" => Ok(color::Magenta.fg_str()),
        "red" => Ok(color::Red.fg_str()),
        "white" => Ok(color::White.fg_str()),
        "yellow" => Ok(color::Yellow.fg_str()),
        _ => {
            return Err(io::Error::new(
                io::ErrorKind::InvalidInput,
                f!("Invalid color: {color_str}"),
            )
            .into())
        }
    };
}

fn clear(subcommand: String, _args: Vec<String>) -> Result<(), ExitFailure> {
    match subcommand.as_str() {
        "all" => print!("{}", termion::clear::All),
        "after_cursor" => print!("{}", termion::clear::AfterCursor),
        "before_cursor" => print!("{}", termion::clear::BeforeCursor),
        "current_line" => print!("{}", termion::clear::CurrentLine),
        "until_newline" => print!("{}", termion::clear::UntilNewline),
        _ => return Err(io::Error::new(io::ErrorKind::InvalidInput, "Unknown clear mode").into()),
    };
    Ok(())
}

fn isatty() -> Result<(), ExitFailure> {
    if stdin_isatty() {
        Ok(())
    } else {
        Err(io::Error::new(io::ErrorKind::InvalidInput, "stdin is not a TTY").into())
    }
}

fn get_cursor_pos(_args: Vec<String>) -> Result<(), ExitFailure> {
    let mut stdout = std::io::stdout().into_raw_mode().unwrap();
    let (x, y) = stdout.cursor_pos()?;
    println!("{x},{y}"); // FIXME: does not print properly
    Ok(())
}

fn get_geometry(_args: Vec<String>) -> Result<(), ExitFailure> {
    let (width, height) = termion::terminal_size().unwrap();
    println!("{width}x{height}");
    Ok(())
}

fn get_height(_args: Vec<String>) -> Result<(), ExitFailure> {
    let (_width, height) = termion::terminal_size().unwrap();
    println!("{height}");
    Ok(())
}

fn get_size(_args: Vec<String>) -> Result<(), ExitFailure> {
    let (width, height) = termion::terminal_size().unwrap();
    println!("{}", width * height);
    Ok(())
}

fn get_width(_args: Vec<String>) -> Result<(), ExitFailure> {
    let (width, _height) = termion::terminal_size().unwrap();
    println!("{width}");
    Ok(())
}

fn move_down(args: Vec<String>) -> Result<(), ExitFailure> {
    print!("{}", termion::cursor::Down(args[0].parse::<u16>().unwrap()));
    Ok(())
}

fn move_left(args: Vec<String>) -> Result<(), ExitFailure> {
    print!("{}", termion::cursor::Left(args[0].parse::<u16>().unwrap()));
    Ok(())
}

fn move_right(args: Vec<String>) -> Result<(), ExitFailure> {
    print!(
        "{}",
        termion::cursor::Right(args[0].parse::<u16>().unwrap())
    );
    Ok(())
}

fn move_to(args: Vec<String>) -> Result<(), ExitFailure> {
    print!(
        "{}",
        termion::cursor::Goto(
            args[0].parse::<u16>().unwrap(),
            args[1].parse::<u16>().unwrap()
        )
    );
    Ok(())
}

fn move_up(args: Vec<String>) -> Result<(), ExitFailure> {
    print!("{}", termion::cursor::Up(args[0].parse::<u16>().unwrap()));
    Ok(())
}

fn print(args: Vec<String>) -> Result<(), ExitFailure> {
    // TODO: parse additional arguments using `clap`

    if args.len() < 3 {
        return Err(io::Error::new(
            io::ErrorKind::InvalidInput,
            f!("Needed arguments not given: fg_color, bg_color, message(s)"),
        )
        .into());
    }

    let fg_color = &args[0];
    let bg_color = &args[1];

    print!(
        "{}{}",
        get_fg_ansi(fg_color.to_string())?,
        get_bg_ansi(bg_color.to_string())?,
    );
    for arg in args.iter().take(args.len() - 1).skip(2) {
        print!("{arg} ");
    }
    println!("{}", args[args.len() - 1]);

    Ok(())
}

fn set_bg_color(args: Vec<String>) -> Result<(), ExitFailure> {
    let color = &args[0]; // TODO: check args is not empty
    print!("{}", get_bg_ansi(color.to_string())?);
    Ok(())
}

fn set_fg_color(args: Vec<String>) -> Result<(), ExitFailure> {
    let color = &args[0]; // TODO: check args is not empty
    print!("{}", get_fg_ansi(color.to_string())?);
    Ok(())
}

fn set_title(args: Vec<String>) -> Result<(), ExitFailure> {
    let title = &args[0]; // TODO: check args is not empty
    print!("\x1b]0;{title}\x07");
    Ok(())
}

fn main() -> Result<(), ExitFailure> {
    let args = Args::parse();

    return match args.command.as_str() {
        "clear" => clear(args.subcommand, args.args),
        "get" => match args.subcommand.as_str() {
            "isatty" => isatty(),
            "cursor_pos" => get_cursor_pos(args.args),
            "geometry" => get_geometry(args.args),
            "height" => get_height(args.args),
            "size" => get_size(args.args),
            "width" => get_width(args.args),
            _ => Err(io::Error::new(
                io::ErrorKind::InvalidInput,
                f!("Invalid argument: subcommand cannot be {args.subcommand}"),
            )
            .into()),
        },
        "move" => match args.subcommand.as_str() {
            "down" => move_down(args.args),
            "left" => move_left(args.args),
            "right" => move_right(args.args),
            "to" => move_to(args.args),
            "up" => move_up(args.args),
            _ => Err(io::Error::new(
                io::ErrorKind::InvalidInput,
                f!("Invalid argument: subcommand cannot be {args.subcommand}"),
            )
            .into()),
        },
        "print" => print(args.args),
        "set" => match args.subcommand.as_str() {
            "bg_color" => set_bg_color(args.args),
            "fg_color" => set_fg_color(args.args),
            "title" => set_title(args.args),
            _ => Err(io::Error::new(
                io::ErrorKind::InvalidInput,
                f!("Invalid argument: subcommand cannot be {args.subcommand}"),
            )
            .into()),
        },
        _ => Err(io::Error::new(
            io::ErrorKind::InvalidInput,
            f!("Invalid argument: command cannot be {args.command}"),
        )
        .into()),
    };
}
