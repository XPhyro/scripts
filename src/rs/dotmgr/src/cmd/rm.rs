use std::path::PathBuf;

use clap::Parser;
use clap::ValueHint::FilePath;
use exitfailure::ExitFailure;

/// Remove files from index.
#[derive(Parser, Debug, Clone)]
#[command(author, version, about, long_about = None)]
#[clap(rename_all = "kebab_case")]
pub struct Args {
    /// Remove directories and their contents recursively
    #[arg(short, long)]
    recursive: bool,

    /// Files to remove
    #[arg(num_args = 1.., value_hint = FilePath, required = true)]
    files: Vec<PathBuf>,
}

pub fn rm(args: Args) -> Result<(), ExitFailure> {
    return Ok(());
}
