use clap::Parser;

/// Get new names for files compliant with the GNU backup syntax.
#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Args {
    /// Use NULs to separate the names instead of newlines.
    #[arg(short = '0', long = "null")]
    nulldelim: bool,

    /// Paths to generate backup paths off of.
    #[arg(required = false)]
    args: Vec<String>,
}

fn main() {
    let args = Args::parse();

    let delim = if args.nulldelim { '\0' } else { '\n' };

    for arg in args.args {
        let path = std::path::Path::new(&arg);
        if !path.exists() {
            print!("{arg}{delim}");
            continue;
        }

        // check if path.~1~ exists, if it does not, print it
        let mut new_path = path.to_path_buf();
        new_path.set_extension("~1~");
        if !new_path.exists() {
            print!("{arg}.~1~{delim}");
            continue;
        }

        // path.~1~ exists, find the next backup number and print it
        let mut backup_number = 2;
        let mut new_path = path.to_path_buf();
        new_path.set_extension(format!("~{backup_number}~"));
        while new_path.exists() {
            backup_number += 1;
            new_path = path.to_path_buf();
            new_path.set_extension(format!("~{backup_number}~"));
        }
        print!("{}{}", new_path.to_str().unwrap(), delim);
    }
}
