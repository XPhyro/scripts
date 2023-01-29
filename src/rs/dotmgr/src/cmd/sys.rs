use clap::{Parser, ValueEnum};
use exitfailure::ExitFailure;

#[derive(ValueEnum, Debug, Clone)]
#[clap(rename_all = "kebab_case")]
enum Subcommand {
    Add,
    Rm,
}

/// Manage systems.
#[derive(Parser, Debug, Clone)]
#[command(author, version, about, long_about = None)]
#[clap(rename_all = "kebab_case")]
pub struct Args {
    /// Command to run
    #[arg(required = true, value_enum)]
    subcommand: Subcommand,

    /// Systems to manage
    #[arg(num_args = 1.., required = true)]
    systems: Vec<String>,
}

pub fn sys(args: Args) -> Result<(), ExitFailure> {
    return Ok(());
}
