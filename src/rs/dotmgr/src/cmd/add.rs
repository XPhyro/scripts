use std::path::PathBuf;

use clap::Parser;
use clap::ValueHint::FilePath;
use exitfailure::ExitFailure;

/// Add files from index.
#[derive(Parser, Debug, Clone)]
#[command(author, version, about, long_about = None)]
#[clap(rename_all = "kebab_case")]
pub struct Args {
    /// Add directories and their contents recursively
    #[arg(short, long)]
    recursive: bool,

    /// Files to add
    #[arg(num_args = 1.., value_hint = FilePath, required = true)]
    files: Vec<PathBuf>,
}

pub fn add(args: Args) -> Result<(), ExitFailure> {
    return Ok(());
}
