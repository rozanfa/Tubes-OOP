#ifndef SLOT_H
#define SLOT_H

#include "../Item/Item.hpp"

class Slot {
    private:
        Item* item;
        int quantity;

    public:
        Slot();
        Slot(Item* item, int quantity);
        Slot(const Slot& slot);
        void operator=(const Slot& slot);
        ~Slot();

        Item* getItem() const;
        int getQuantity() const;

        void add(Item* item, int qt);
        void add(Slot& slot);
        Slot remove(int qt);
        void clear();
        void print() const;
};

class ItemMismatchException : public exception {
    public:
        const char *what() const throw() {
            return "This item has different id";
        }
};

class ItemNotEnoughException : public exception {
    public:
        const char *what() const throw() {
            return "This item has not enough quantity";
        }
};

class SlotEmptyException : public exception {
    public:
        const char *what() const throw() {
            return "This slot is empty";
        }
};

class SlotFullException : public exception {
    public:
        const char *what() const throw() {
            return "This item can't be stacked more than 64";
        }
};

class ToolItemStackedException : public exception {
    public:
        const char *what() const throw() {
            return "Tool item can't be stacked";
        }
};

#endif