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

    /// Bake the bread with a specific width.
    #[arg(short = 'w', long = "width")]
    width: Option<u16>,

    /// Dough for the bread.
    #[arg(required = true)]
    dough: String,

    /// Ingredients to put between the bread. If given, stdin is used after positional ingredients.
    #[arg(required = false)]
    ingredients: Vec<String>,
}

fn bake_bread(width: u16, dough: &str, random_dough: bool) {
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

    let width = if args.width.is_some() {
        args.width.unwrap()
    } else {
        termion::terminal_size().unwrap().0
    };

    bake_bread(width, &args.dough, args.random_dough);

    println!("{ingredients}");
    if !stdin_isatty() {
        std::io::copy(&mut std::io::stdin().lock(), &mut std::io::stdout().lock()).unwrap();
    }

    bake_bread(width, &args.dough, args.random_dough);
}
