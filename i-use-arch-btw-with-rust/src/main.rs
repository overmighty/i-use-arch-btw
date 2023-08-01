use std::env::args;
use std::fmt::Write;
use std::fs::File;
use std::io::{self, Read, Write as W};

struct Interpreter<'a> {
    memory: [u8; 65536],
    data_pointer: usize,
    source_code: Vec<&'a str>,
}

impl<'a> Interpreter<'a> {
    fn new(source_code: Vec<&'a str>) -> Self {
        Interpreter {
            memory: [0u8; 65536],
            data_pointer: 0,
            source_code,
        }
    }

    fn execute(&mut self) {
        let mut i = 0;

        while i < self.source_code.len() {
            match self.source_code[i] {
                "i" => self.increment_data_pointer(),
                "use" => self.decrement_data_pointer(),
                "arch" => self.increment_value(),
                "linux" => self.decrement_value(),
                "btw" => self.write_to_output(),
                "by" => self.read_from_input(),
                "the" => {
                    if self.memory[self.data_pointer] == 0 {
                        let mut count = 1;
                        while count > 0 {
                            i += 1;
                            if self.source_code[i] == "the" {
                                count += 1;
                            } else if self.source_code[i] == "way" {
                                count -= 1;
                            }
                        }
                    }
                }
                "way" => {
                    if self.memory[self.data_pointer] != 0 {
                        let mut count = 1;
                        while count > 0 {
                            i -= 1;
                            if self.source_code[i] == "way" {
                                count += 1;
                            } else if self.source_code[i] == "the" {
                                count -= 1;
                            }
                        }
                    }
                }
                "gentoo" => self.debug_handler(),
                _ => eprintln!("Unknown keyword"),
            }
            i += 1;
        }
    }

    fn increment_data_pointer(&mut self) {
        if self.data_pointer < self.memory.len() - 1 {
            self.data_pointer += 1;
        }
    }

    fn decrement_data_pointer(&mut self) {
        if self.data_pointer > 0 {
            self.data_pointer -= 1;
        }
    }

    fn increment_value(&mut self) {
        self.memory[self.data_pointer] = self.memory[self.data_pointer].wrapping_add(1);
    }

    fn decrement_value(&mut self) {
        self.memory[self.data_pointer] = self.memory[self.data_pointer].wrapping_sub(1);
    }

    fn write_to_output(&self) {
        let byte = self.memory[self.data_pointer];
        let character = char::from(byte);
        print!("{}", character);
        io::stdout().flush().unwrap();
    }

    fn read_from_input(&mut self) {
        let mut buffer = [0u8; 1];
        io::stdin().read_exact(&mut buffer).unwrap();
        self.memory[self.data_pointer] = buffer[0];
    }

    fn debug_handler(&self) {
        let data_pointer_addr = self.source_code.as_ptr() as usize + self.data_pointer;
        let memory_addr = self.memory.as_ptr() as usize + self.data_pointer;
        let memory_value = self.memory[self.data_pointer];

        let mut debug_output = String::new();
        write!(
                            &mut debug_output,
                                "\x1b[1;34mdebug: \x1b[0mip \x1b[36m=\x1b[0m \x1b[33m0x{:x}\x1b[0m \x1b[90m(\x1b[0mprogram \x1b[36m+\x1b[0m \x1b[33m0x{:x}\x1b[0m\x1b[90m)\x1b[36m;\x1b[0m dp\x1b[36m\x1b[0m \x1b[36m=\x1b[0m \x1b[33m0x{:x}\x1b[0m \x1b[90m(\x1b[0mmemory \x1b[36m+\x1b[0m \x1b[33m0x{:x}\x1b[0m\x1b[90m)\x1b[0m\x1b[36m;\x1b[0m \x1b[36m*\x1b[0mdp \x1b[36m=\x1b[0m \x1b[33m{}\x1b[0m",
                            data_pointer_addr,
                            self.data_pointer,
                            memory_addr,
                            self.data_pointer,
                            memory_value
                        )
            .unwrap();
        println!("{}", debug_output);
    }
}

fn main() {
    // Get the input file name from the command-line arguments
    let input_filename = args().nth(1).expect("Usage: i-use-arch-btw <source file>");

    let mut source_code = String::new();
    let mut input_file = match File::open(input_filename) {
        Ok(file) => file,
        Err(err) => {
            eprintln!("Error opening the file: {}", err);
            return;
        }
    };
    if let Err(err) = input_file.read_to_string(&mut source_code) {
        eprintln!("Error reading from file: {}", err);
        return;
    }

    let mut tokens: Vec<&str> = Vec::new();

    for line in source_code.lines() {
        // Ignore lines that start with a semicolon (comments)
        if !line.trim_start().starts_with(';') {
            // Split each line by semicolon to remove comments
            let line_tokens: Vec<&str> =
                line.split(';').next().unwrap().split_whitespace().collect();
            tokens.extend(line_tokens);
        }
    }

    let mut interpreter = Interpreter::new(tokens);
    interpreter.execute();
}
