# Inventory & E-Commerce Management System (C++)

A console-based store management system written in C++ that supports three
user roles — **Admin**, **Employee**, and **Customer** — each with its own menu
and permissions. Data is persisted in plain-text files that act as a simple
flat-file database, and string handling is implemented manually rather than
using the C++ string library.

This was my first programming project. It focuses on the fundamentals: file
input/output, control flow, functions, character arrays, and structuring a
larger program into reusable pieces.

---

## Features

### Admin
- User & role management (add, modify, delete accounts)
- Product management (add / remove products, edit stock)
- Announcement management (view, add, delete)
- Activity log viewer (every login and key action is recorded)
- Sales & revenue analysis (total sales + best-selling product)
- Audit trail of account changes

### Employee
- View and respond to customer support requests
- Inventory management (add / remove products, update quantities)
- View and respond to product feedback
- Post and manage announcements
- Low-stock alerts shown automatically on login

### Customer
- Browse the catalog with filtering by name, price range, category, or product ID
- Cart management (add, remove, update quantity, view totals)
- Checkout with discount-code support and stock validation
- Wishlist (add, remove, view)
- Order history and the ability to leave feedback on purchased items
- Send support requests

---

## Technical Highlights

- **Role-based access control** — a single entry menu routes each user to a
  separate function (`admin`, `employee`, `customer`) with its own capabilities.
- **Flat-file persistence** — products, users, carts, orders, wishlists,
  announcements, logs, and feedback are each stored in their own `.txt` file and
  updated using a read-into-temp-file then rename pattern.
- **Manual string utilities** — `arrayComparison` and `manualStrcpy` implement
  comparison and copying on raw character arrays without `<cstring>` or
  `std::string`. A small `cleanField` helper strips BOM / `\r` so the program
  reads data files correctly regardless of how they were saved.
- **Activity logging & audit trail** — security-relevant events (logins, failed
  logins, account changes, inventory changes) are written to log files.

---

## Getting Started

### Prerequisites
- A C++ compiler (e.g. `g++`)

### Clone, build and run
```bash
git clone https://github.com/<your-username>/inventory-system.git
cd inventory-system
g++ -std=c++11 -o store main.cpp
./store
```

> Run `./store` from the project folder so it can find the data files that sit
> next to it.

### Default logins (from the sample data)

| Role     | Username | Password |
|----------|----------|----------|
| Admin    | `adm`    | `adm`    |
| Employee | `ss`     | `ss`     |
| Customer | `subhan` | `subhan` |

Customers can also create a new account from the Customer menu.

---

## Project Structure

| File               | Tracked? | Purpose                                            |
|--------------------|----------|----------------------------------------------------|
| `main.cpp`         | yes      | All program logic                                  |
| `verification.txt` | yes      | User accounts: `name,password,role`                |
| `Catalog.txt`      | yes      | Products: `name price stock category id`           |
| `orderhis.txt`     | yes      | Completed orders (sample data)                     |
| `discount.txt`     | yes      | Promo codes: `code percentage`                     |
| `announcement.txt` | yes      | Announcements: `name\|message`                     |
| `Cart.txt`         | no       | Active cart items (generated at runtime)           |
| `wishlist.txt`     | no       | Saved wishlist items (generated at runtime)        |
| `feedback.txt`     | no       | Product feedback (generated at runtime)            |
| `supportreq.txt`   | no       | Support requests (generated at runtime)            |
| `ActivityLogs.txt` | no       | Login / action history (generated at runtime)      |
| `AuditTrail.txt`   | no       | Admin account-change history (generated at runtime)|

The files marked "no" are listed in `.gitignore`; the program creates them
automatically the first time it needs them, so a fresh clone runs without them.

---

## Possible Future Improvements

Ideas I'd explore to take this further:

- **Password security** — passwords are currently stored in plain text; hashing
  them would be the natural next step.
- **Input validation** — guard against non-numeric input on menu prompts so a
  stray keystroke can't disrupt the menu loop.
- **Move to `std::string` and `struct`s** — replacing fixed-size character
  arrays and parallel reads with structured records would make the code safer
  and shorter.
- **Single data-access layer** — the read-temp-rename logic is repeated in many
  places and could be centralised into a few helper functions.

---

## License

Released under the [MIT License](LICENSE).
