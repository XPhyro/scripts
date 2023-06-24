use clap::Parser;

/// Get new names for files compliant with the GNU backup syntax.
#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Args {
    /// Positional arguments for the subcommand.
    #[arg(required = false)]
    args: Vec<String>,
}

fn main() {
    let args = Args::parse();

    for arg in args.args {
        let path = std::path::Path::new(&arg);
        if !path.exists() {
            println!("{}", arg);
            continue;
        }

        // check if path.~1~ exists, if it does not, print it
        let mut new_path = path.to_path_buf();
        new_path.set_extension("~1~");
        if !new_path.exists() {
            println!("{}.~1~", arg);
            continue;
        }

        // path.~1~ exists, find the next backup number and print it
        let mut backup_number = 2;
        new_path = path.to_path_buf();
        new_path.set_extension(format!("~{}~", backup_number));
        while new_path.exists() {
            backup_number += 1;
            new_path = path.to_path_buf();
            new_path.set_extension(format!("~{}~", backup_number));
        }
        println!("{}", new_path.to_str().unwrap());
    }
}
