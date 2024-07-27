use clap::Parser;

/// Slice a list of arguments.
#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Args {
    /// Python-like slice expression.
    #[arg(required = true)]
    slice: String,

    /// Arguments to slice. If not present, stdin is used.
    #[arg(required = false)]
    args: Vec<String>,
}

fn logerrq(err: impl std::fmt::Display) -> ! {
    let execname = std::env::args()
        .next()
        .and_then(|p| p.rsplit('/').next().map(|s| s.to_string()))
        .unwrap_or_else(|| "argn".to_string());

    eprintln!("{}: {}", execname, err);
    std::process::exit(1);
}

fn parse_part(part: Option<&str>, default: isize, name: &str) -> isize {
    match part {
        Some(p) if !p.trim().is_empty() => p
            .parse()
            .unwrap_or_else(|_| logerrq(format!("invalid {} value: {}", name, p))),
        _ => default,
    }
}

fn parse_slice(slice: &str) -> (isize, isize, isize) {
    let parts: Vec<&str> = slice.split(':').collect();
    let begin = parse_part(parts.get(0).copied(), 0, "begin");
    let end = parse_part(parts.get(1).copied(), isize::MAX, "end");
    let increment = parse_part(parts.get(2).copied(), 1, "increment");
    (begin, end, increment)
}

fn apply_slice<T>(input: &[T], begin: isize, end: isize, increment: isize) -> Vec<T>
where
    T: Clone,
{
    let len = input.len() as isize;
    let mut result = Vec::new();

    let start = if begin < 0 {
        (len + begin).max(0)
    } else {
        begin.min(len)
    } as usize;

    let stop = if end < 0 {
        (len + end).max(0)
    } else {
        end.min(len)
    } as usize;

    if increment > 0 {
        let mut i = start;
        while i < stop {
            result.push(input[i].clone());
            i = (i as isize + increment) as usize;
        }
    } else if increment < 0 {
        let mut i = start;
        while i > stop {
            result.push(input[i].clone());
            i = (i as isize + increment) as usize;
        }
    }

    result
}

fn main() {
    let args = Args::parse();

    let (begin, end, increment) = parse_slice(&args.slice);

    let input = if args.args.is_empty() {
        use std::io::{self, BufRead};
        let stdin = io::stdin();
        stdin
            .lock()
            .lines()
            .map(|line| line.unwrap())
            .collect::<Vec<_>>()
    } else {
        args.args
    };

    let sliced_args = apply_slice(&input, begin, end, increment);
    let separator = "\n";
    print!("{}{}", sliced_args.join(separator), separator);
}
