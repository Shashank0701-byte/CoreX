# How to Push CoreX OS to GitHub

## Step 1: Initialize Git Repository
```bash
cd /d/ResumeProjects/CoreX
git init
git add .
git commit -m "Initial commit: CoreX OS - Custom x86 Operating System

Features:
- Custom bootloader in x86 Assembly
- 32-bit protected mode kernel in C
- IDT with exception and IRQ handlers
- Physical memory manager with paging support
- VGA text mode driver
- PS/2 keyboard driver
- Interactive shell with built-in commands
- In-memory file system
"
```

## Step 2: Create GitHub Repository
1. Go to https://github.com/new
2. Repository name: `CoreX-OS` or `custom-x86-os`
3. Description: "A custom 32-bit x86 operating system built from scratch in C and Assembly"
4. Make it **Public** (for resume visibility)
5. **Don't** initialize with README (we already have one)
6. Click "Create repository"

## Step 3: Push to GitHub
```bash
git remote add origin https://github.com/YOUR_USERNAME/CoreX-OS.git
git branch -M main
git push -u origin main
```

## Step 4: Add Topics/Tags on GitHub
After pushing, go to your repository page and add these topics:
- `operating-system`
- `os-development`
- `x86`
- `assembly`
- `c`
- `kernel`
- `bootloader`
- `osdev`

## Step 5: Take Screenshots
Before pushing, take screenshots of:
1. OS booting (showing initialization messages)
2. Shell interface (showing the prompt)
3. Demo mode running (if you implement it)

Save them in a `docs/` folder and reference them in README.md

## Step 6: Add to Resume
On your resume, add:

**CoreX OS** | [GitHub Link]
- Developed a custom 32-bit x86 operating system from scratch using C and x86 Assembly
- Implemented bootloader, protected mode kernel, interrupt handling, and memory management
- Built VGA driver, keyboard driver, and interactive shell with command parsing
- Demonstrates deep understanding of low-level systems programming and OS architecture

## Optional: Create a Demo Video
Record a quick video showing:
1. Building the OS
2. Running in QEMU
3. OS booting
4. Shell displaying

Upload to YouTube and link in README!
