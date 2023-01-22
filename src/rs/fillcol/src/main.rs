extern crate termion;

use std::env;
use std::io;

use crossterm::cursor;

fn main() -> io::Result<()> {
    if env::args().len() < 3 {
        return Err(io::Error::new(
            io::ErrorKind::InvalidInput,
            "Needed arguments: COLUMN and STRING",
        ));
    }

    let (term_width, term_height) = termion::terminal_size().unwrap();
    let mut col = env::args().nth(1).unwrap().parse::<u16>().unwrap();
    let msg = env::args().nth(2).unwrap();

    if col > term_width {
        col = term_width - msg.len() as u16 + 1;
    } else if col < 1 {
        col = 1;
    }

    let cursor = cursor();

    cursor.save_position();

    for line in 1..term_height {
        print!("{}{}", termion::cursor::Goto(col, line), msg);
    }

    cursor.restore_position();

    Ok(())
}
