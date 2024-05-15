extern crate isatty;

use clap::{Args, Parser, Subcommand, ValueEnum};
use etcetera::{choose_base_strategy, BaseStrategy};
use isatty::{stdin_isatty, stdout_isatty};
use once_cell::sync::OnceCell;
use std::fs::File;
use std::path::PathBuf;

#[derive(Parser)]
#[command(version, about, long_about = None)]
#[command(propagate_version = true)]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    /// Print aliases evalable by a POSIX-compatible shell.
    Alias(AliasArgs),

    /// Mark the given directory or file.
    Mark(MarkArgs),

    /// Unmark the given keycode, directory or file.
    Unmark(UnmarkArgs),

    /// Mark or unmark the given keycode, directory or file.
    ToggleMark(ToggleMarkArgs),

    /// Jump to the directory for the given keycode.
    Jump(JumpArgs),

    /// Open the file for the given keycode.
    Open(OpenArgs),

    /// Print the value for the given keycode.
    Print(PrintArgs),

    /// List all marks.
    List(ListArgs),

    /// Manually edit the database.
    Edit(EditArgs),
}

#[derive(Args)]
struct AliasArgs {}

#[derive(Args)]
struct MarkArgs {
    /// Don't require manual confirmation for overwriting marks.
    #[arg(short, long)]
    yes: bool,

    /// Keycode to assign the file to.
    keycode: String,

    /// Directory or file to mark. If not given, $PWD is used.
    file: Option<String>,
}

#[derive(Args)]
struct UnmarkArgs {
    /// Keycode, directory or file to unmark.
    identifier: String,
}

#[derive(Args)]
struct ToggleMarkArgs {
    /// Don't require manual confirmation for overwriting marks.
    #[arg(short, long)]
    yes: bool,

    /// Keycode, directory or file to mark or unmark.
    identifier: String,

    /// Directory or file to mark. If not given, $PWD is used.
    file: Option<String>,
}

#[derive(Args)]
struct JumpArgs {
    /// Keycode to match the directory.
    keycode: String,
}

#[derive(Args)]
struct OpenArgs {
    /// Keycode to match the file.
    keycode: String,
}

#[derive(Args)]
struct PrintArgs {
    /// Database to search.
    #[clap(short, long, value_enum, default_value_t = Databases::Both)]
    database: Databases,

    /// Keycode to print the value.
    keycode: String,
}

#[derive(Args)]
struct ListArgs {
    /// Database to list.
    #[clap(short, long, value_enum, default_value_t = Databases::Both)]
    database: Databases,

    /// Use NUL as output delimiter.
    #[arg(short = 'z', long)]
    null: bool,
}

#[derive(Args)]
struct EditArgs {
    /// Database to edit.
    #[clap(short, long, value_enum, default_value_t = Database::Directory)]
    database: Database,
}

#[derive(Copy, Clone, PartialEq, Eq, PartialOrd, Ord, ValueEnum)]
enum Database {
    Directory,
    File,
}

#[derive(Copy, Clone, PartialEq, Eq, PartialOrd, Ord, ValueEnum)]
enum Databases {
    Both,
    Directory,
    File,
}

fn main() {
    let cli = Cli::parse();

    match &cli.command {
        Commands::Alias(cmd) => cmd_alias(cmd),
        Commands::Mark(cmd) => cmd_mark(cmd),
        Commands::Unmark(cmd) => cmd_unmark(cmd),
        Commands::ToggleMark(cmd) => cmd_toggle_mark(cmd),
        Commands::Jump(cmd) => cmd_jump(cmd),
        Commands::Open(cmd) => cmd_open(cmd),
        Commands::Print(cmd) => cmd_print(cmd),
        Commands::List(cmd) => cmd_list(cmd),
        Commands::Edit(cmd) => cmd_edit(cmd),
    }
}

fn get_cache_dir() -> &'static PathBuf {
    static CACHE_DIR: OnceCell<PathBuf> = OnceCell::new();
    CACHE_DIR.get_or_init(|| {
        let strategy = choose_base_strategy().unwrap();
        let mut cache_dir = strategy.cache_dir();
        cache_dir.push("scripts/pathfinder/");
        std::fs::create_dir_all(&cache_dir).unwrap();
        cache_dir
    })
}

fn get_directory_db() -> &'static File {
    static DIRECTORY_DB: OnceCell<File> = OnceCell::new();
    DIRECTORY_DB.get_or_init(|| {
        let mut directory_db_path = get_cache_dir().clone();
        directory_db_path.push("directories");
        File::create(directory_db_path).expect("Failed to create directory database.")
    })
}

fn get_file_db() -> &'static File {
    static FILE_DB: OnceCell<File> = OnceCell::new();
    FILE_DB.get_or_init(|| {
        let mut file_db_path = get_cache_dir().clone();
        file_db_path.push("files");
        File::create(file_db_path).expect("Failed to create file database.")
    })
}

fn get_directory_hashmap() {
    let db = get_directory_db();
    todo!();
}

fn get_file_hashmap() {
    let db = get_file_db();
    todo!();
}

fn cmd_alias(_args: &AliasArgs) {
    println!(
        r#"m() eval "$(pathfinder toggle-mark "$@")"
g() eval "$(pathfinder jump "$@")"
e() eval "$(pathfinder open "$@")""#
    );
}

fn cmd_mark(args: &MarkArgs) {
    todo!()
}

fn cmd_unmark(args: &UnmarkArgs) {
    todo!()
}

fn cmd_toggle_mark(args: &ToggleMarkArgs) {
    todo!()
}

fn cmd_jump(args: &JumpArgs) {
    todo!()
}

fn cmd_open(args: &OpenArgs) {
    todo!()
}

fn cmd_print(args: &PrintArgs) {
    let file = match args.database {
        Databases::Both => todo!(),
        Databases::Directory => todo!(),
        Databases::File => todo!(),
    };

    if stdout_isatty() {
        todo!()
        // println!("{}", file);
    } else {
        todo!()
        // print!("{}", file);
    }
}

fn cmd_list(args: &ListArgs) {
    if !stdout_isatty() {
        todo!()
    } else {
        todo!()
    }
}

fn cmd_edit(args: &EditArgs) {
    todo!()
}
