pub mod cmd;
pub mod util;

use clap::{Parser, ValueEnum};
use exitfailure::ExitFailure;

#[derive(ValueEnum, Debug, Clone)]
#[clap(rename_all = "kebab_case")]
enum Command {
    Add,
    Rm,
    Sys,
}

/// Manage dotfiles.
#[derive(Parser, Debug, Clone)]
#[command(author, version, about, long_about = None)]
#[clap(rename_all = "kebab_case")]
struct Args {
    /// Command to run.
    #[arg(required = true, value_enum)]
    command: Command,

    /// Command arguments.
    #[arg(required = false)]
    #[clap(trailing_var_arg = true, allow_hyphen_values = true)]
    args: Vec<String>,
}

fn main() -> Result<(), ExitFailure> {
    let args = Args::parse();

    match args.command {
        Command::Add => {
            let mut add_args = vec!["add".to_string()];
            add_args.extend(args.args);
            let add_args = cmd::add::Args::parse_from(add_args);
            return cmd::add::add(add_args);
        }
        Command::Rm => {
            let mut rm_args = vec!["rm".to_string()];
            rm_args.extend(args.args);
            let rm_args = cmd::rm::Args::parse_from(rm_args);
            return cmd::rm::rm(rm_args);
        }
        Command::Sys => {
            let mut sys_args = vec!["sys".to_string()];
            sys_args.extend(args.args);
            let sys_args = cmd::sys::Args::parse_from(sys_args);
            return cmd::sys::sys(sys_args);
        }
    }
}
