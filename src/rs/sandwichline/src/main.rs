extern crate isatty;
extern crate termion;

use clap::Parser;
use isatty::stdin_isatty;
use rand::Rng;

/// Make a custom sandwich.
#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Args {
    /// Mix the dough randomly.
    #[arg(short = 'r', long = "random-dough")]
    random_dough: bool,

    /// Dough for the bread.
    #[arg(required = true)]
    dough: String,

    /// Ingredients to put between the bread.
    #[arg(required = false)]
    ingredients: Vec<String>,
}

fn make_bread(width: u16, dough: &str, random_dough: bool) {
    if random_dough {
        for _ in 0..width {
            print!(
                "{}",
                dough
                    .chars()
                    .nth(rand::thread_rng().gen_range(0..dough.len()))
                    .unwrap()
            );
        }
    } else {
        for i in 0..width {
            print!("{}", dough.chars().nth((i as usize) % dough.len()).unwrap());
        }
    }
    println!();
}

fn main() {
    let args = Args::parse();

    let ingredients = args.ingredients.join(" ");
    let (width, _height) = termion::terminal_size().unwrap();

    make_bread(width, &args.dough, args.random_dough);

    println!("{}", ingredients);
    if !stdin_isatty() {
        std::io::copy(&mut std::io::stdin().lock(), &mut std::io::stdout().lock()).unwrap();
    }

    make_bread(width, &args.dough, args.random_dough);
}
