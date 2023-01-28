pub mod util;

use clap::{Parser, ValueEnum};
use exitfailure::ExitFailure;

#[derive(ValueEnum, Debug, Clone)]
#[clap(rename_all = "kebab_case")]
enum Command {
    Add,
    Rm,
}

/// Add files from index.
#[derive(Parser, Debug, Clone)]
#[command(author, version, about, long_about = None)]
#[clap(rename_all = "kebab_case")]
struct AddArgs {
    /// Files to add.
    #[arg(num_args = 1.., required = true)]
    files: Vec<String>,
}

fn add(args: AddArgs) -> Result<(), ExitFailure> {
    return Ok(());
}

/// Remove files from index.
#[derive(Parser, Debug, Clone)]
#[command(author, version, about, long_about = None)]
#[clap(rename_all = "kebab_case")]
struct RmArgs {
    /// Files to remove.
    #[arg(num_args = 1.., required = true)]
    files: Vec<String>,
}

fn rm(args: RmArgs) -> Result<(), ExitFailure> {
    return Ok(());
}

/// Manage dotfiles.
#[derive(Parser, Debug, Clone)]
#[command(author, version, about, long_about = None)]
#[clap(rename_all = "kebab_case")]
struct Args {
    /// Command to run.
    #[arg(required = true, value_enum)]
    command: Command,

    /// Positional arguments.
    #[arg(required = false)]
    args: Vec<String>,
}

fn main() -> Result<(), ExitFailure> {
    let args = Args::parse();

    match args.command {
        Command::Add => {
            let mut add_args = vec!["add".to_string()];
            add_args.extend(args.args);
            let add_args = AddArgs::parse_from(add_args);
            return add(add_args);
        }
        Command::Rm => {
            let mut rm_args = vec!["rm".to_string()];
            rm_args.extend(args.args);
            let rm_args = RmArgs::parse_from(rm_args);
            return rm(rm_args);
        }
    }
}
