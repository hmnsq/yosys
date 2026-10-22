/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2020  Marcelina Kościelnicka <mwk@0x04.net>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ifndef MEM_H
#define MEM_H

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/ffinit.h"
#અન્તર્ભાવ "kernel/utils.h"

YOSYS_NAMESPACE_BEGIN

સંરચના MemRd : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	પૂર્ણાંક wide_log2;
	bool clk_enable, clk_polarity, ce_over_srst;
	Const arst_કિંમત, srst_કિંમત, init_કિંમત;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will bypass the written data
	// to this port's output (default behavior is to read old value).
	// Can only be set for write ports that have the same clock domain.
	std::vector<bool> transparency_mask;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will return an all-X (don't care)
	// value.  Mutually exclusive with transparency_mask.
	// Can only be set for write ports that have the same clock domain.
	// For optimization purposes, this will also be set if we can
	// determine that the two ports can never be active simultanously
	// (making the above vacuously true).
	std::vector<bool> collision_x_mask;
	SigSpec clk, en, arst, srst, addr, data;

	MemRd() : removed(false), cell(nullptr), wide_log2(0), clk_enable(false), clk_polarity(true), ce_over_srst(false), clk(State::Sx), en(State::S1), arst(State::S0), srst(State::S0) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(પૂર્ણાંક sub) {
		SigSpec res = addr;
		માટે (પૂર્ણાંક i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		પાછા res;
	}
};

સંરચના MemWr : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	પૂર્ણાંક wide_log2;
	bool clk_enable, clk_polarity;
	std::vector<bool> priority_mask;
	SigSpec clk, en, addr, data;

	MemWr() : removed(false), cell(nullptr) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(પૂર્ણાંક sub) {
		SigSpec res = addr;
		માટે (પૂર્ણાંક i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		પાછા res;
	}

	std::pair<SigSpec, std::vector<પૂર્ણાંક>> compress_en();
	SigSpec decompress_en(અચલ std::vector<પૂર્ણાંક> &swizzle, SigSpec sig);
};

સંરચના MemInit : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	Const addr;
	Const data;
	Const en;
	MemInit() : removed(false), cell(nullptr) {}
};

સંરચના Mem : RTLIL::AttrObject {
	Module *module;
	PooledName memid;
	bool packed;
	RTLIL::Memory *mem;
	Cell *cell;
	પૂર્ણાંક width, start_offset, size;
	std::vector<MemInit> inits;
	std::vector<MemRd> rd_ports;
	std::vector<MemWr> wr_ports;

	// Removes this memory from the module.  The data in helper structures
	// is unaffected except for the cell/mem fields.
	શૂન્ય remove();

	// Commits all changes in helper structures into the module — ports and
	// inits marked as removed are actually removed, new ports/inits create
	// new cells, modified port/inits are commited into their existing
	// cells.  Note that this reindexes the ports and inits array (actually
	// removing the ports/inits marked as removed).
	શૂન્ય emit();

	// Marks all inits as removed.
	શૂન્ય clear_inits();

	// Coalesces inits: whenever two inits have overlapping or touching
	// address ranges, they are combined into one, with the higher-priority
	// one's data overwriting the other.  Running this results in
	// an inits list equivalent to the original, in which all entries
	// cover disjoint (and non-touching) address ranges, and all enable
	// masks are all-1.
	શૂન્ય coalesce_inits();

	// Checks consistency of this memory and all its ports/inits, using
	// log_assert.
	શૂન્ય check();

	// Gathers all initialization data into a single big const covering
	// the whole memory.  For all non-initialized bits, Sx will be returned.
	Const get_init_data() અચલ;

	// Constructs and returns the helper structures for all memories
	// in a module.
	સ્થિર std::vector<Mem> get_all_memories(Module *module);

	// Constructs and returns the helper structures for all selected
	// memories in a module.
	સ્થિર std::vector<Mem> get_selected_memories(Module *module);

	// Converts a synchronous read port into an asynchronous one by
	// extracting the data (or, in some rare cases, address) register
	// into a separate cell, together with any soft-transparency
	// logic necessary to preserve its semantics.  Returns the created
	// register cell, if any.  Note that in some rare cases this function
	// may succeed and perform a conversion without creating a new
	// register — a nullptr result doesn't imply nothing was done.
	Cell *extract_rdff(પૂર્ણાંક idx, FfInitVals *initvals);

	// Splits all wide ports in this memory into equivalent narrow ones.
	// This function performs no modifications at all to the actual
	// netlist unless and until emit() is called.
	શૂન્ય narrow();

	// If write port idx2 currently has priority over write port idx1,
	// inserts extra logic on idx1's enable signal to disable writes
	// when idx2 is writing to the same address, then removes the priority
	// from the priority mask.  If there is a memory port that is
	// transparent with idx1, but not with idx2, that port is converted
	// to use soft transparency logic.
	શૂન્ય emulate_priority(પૂર્ણાંક idx1, પૂર્ણાંક idx2, FfInitVals *initvals);

	// Creates soft-transparency logic on read port ridx, bypassing the
	// data from write port widx.  Should only be called when ridx is
	// transparent wrt widx in the first place.  Once we're done, the
	// transparency_mask bit will be cleared, and the collision_x_mask
	// bit will be set instead (since whatever value is read will be
	// replaced by the soft transparency logic).
	શૂન્ય emulate_transparency(પૂર્ણાંક widx, પૂર્ણાંક ridx, FfInitVals *initvals);

	// Prepares for merging write port idx2 into idx1 (where idx1 < idx2).
	// Specifically, takes care of priority masks: any priority relations
	// that idx2 had are replicated onto idx1, unless they conflict with
	// priorities already present on idx1, in which case emulate_priority
	// is called.  Likewise, ensures transparency and undefined collision
	// masks of all read ports have the same values for both ports,
	// calling emulate_transparency if necessary.
	શૂન્ય prepare_wr_merge(પૂર્ણાંક idx1, પૂર્ણાંક idx2, FfInitVals *initvals);

	// Prepares for merging read port idx2 into idx1.
	// Specifically, makes sure the transparency and undefined collision
	// masks of both ports are equal, by changing undefined behavior
	// of one port to the other's defined behavior, or by calling
	// emulate_transparency if necessary.
	શૂન્ય prepare_rd_merge(પૂર્ણાંક idx1, પૂર્ણાંક idx2, FfInitVals *initvals);

	// Prepares the memory for widening a port to a given width.  This
	// involves ensuring that start_offset and size are aligned to the
	// target width.
	શૂન્ય widen_prep(પૂર્ણાંક wide_log2);

	// Widens a write port up to a given width.  The newly port is
	// equivalent to the original, made by replicating enable/data bits
	// and masking enable bits with decoders on the low part of the
	// original address.
	શૂન્ય widen_wr_port(પૂર્ણાંક idx, પૂર્ણાંક wide_log2);

	// Emulates a sync read port's enable functionality in soft logic,
	// changing the actual read port's enable to be always-on.
	શૂન્ય emulate_rden(પૂર્ણાંક idx, FfInitVals *initvals);

	// Emulates a sync read port's initial/reset value functionality in
	// soft logic, removing it from the actual read port.
	શૂન્ય emulate_reset(પૂર્ણાંક idx, bool emu_init, bool emu_arst, bool emu_srst, FfInitVals *initvals);

	// Given a read port with ce_over_srst set, converts it to a port
	// with ce_over_srst unset without changing its behavior by adding
	// emulation logic.
	શૂન્ય emulate_rd_ce_over_srst(પૂર્ણાંક idx);

	// Given a read port with ce_over_srst unset, converts it to a port
	// with ce_over_srst set without changing its behavior by adding
	// emulation logic.
	શૂન્ય emulate_rd_srst_over_ce(પૂર્ણાંક idx);

	// Returns true iff emulate_read_first makes sense to call.
	bool emulate_read_first_ok();

	// Emulates all read-first read-write port relationships in terms of
	// all-transparent ports, by delaying all write ports by one cycle.
	// This can only be used when all read ports and all write ports are
	// in the same clock domain.
	શૂન્ય emulate_read_first(FfInitVals *initvals);

	Mem(Module *module, IdString memid, પૂર્ણાંક width, પૂર્ણાંક start_offset, પૂર્ણાંક size) : module(module), memid(module->design, memid), packed(false), mem(nullptr), cell(nullptr), width(width), start_offset(start_offset), size(size) {}
	Mem(Module *module, TwineSpec &&memid, પૂર્ણાંક width, પૂર્ણાંક start_offset, પૂર્ણાંક size)
			: Mem(module, module->twines().add(std::move(memid)), width, start_offset, size) {}
};

// MemContents efficiently represents the contents of a potentially sparse memory by storing only those segments that are actually defined
class MemContents {
જાહેર:
	class range; class iterator;
	using addr_t = uint32_t;
ખાનગી:
	// we ban _addr_width == sizeof(addr_t) * 8 because it adds too many cornercases
	પૂર્ણાંક _addr_width;
	પૂર્ણાંક _data_width;
	RTLIL::Const default_કિંમત;
	// for each range, store the concatenation of the words at the start address
	// invariants:
	// - no overlapping or adjacent ranges
	// - no empty ranges
	// - all Consts are a multiple of the word size
	std::map<addr_t, RTLIL::Const> કિંમતો_2;
	// returns an iterator to the range containing addr, if it exists, or the first range past addr
	std::map<addr_t, RTLIL::Const>::iterator _range_at(addr_t addr) અચલ;
	addr_t _range_size(std::map<addr_t, RTLIL::Const>::iterator it) અચલ { પાછા it->second.size() / _data_width; }
	addr_t _range_begin(std::map<addr_t, RTLIL::Const>::iterator it) અચલ { પાછા it->first; }
	addr_t _range_end(std::map<addr_t, RTLIL::Const>::iterator it) અચલ { પાછા _range_begin(it) + _range_size(it); }
	// check if the iterator points to a range containing addr
	bool _range_contains(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) અચલ;
	// check if the iterator points to a range containing [begin_addr, end_addr). assumes end_addr >= begin_addr.
	bool _range_contains(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t end_addr) અચલ;
	// check if the iterator points to a range overlapping with [begin_addr, end_addr)
	bool _range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t end_addr) અચલ;
	// return the offset the addr would have in the range at `it`
	size_t _range_offset(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) અચલ { પાછા (addr - it->first) * _data_width; }
	// assuming _range_contains(it, addr), return an iterator pointing to the data at addr
	RTLIL::Const::iterator _range_data(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) {
		પાછા RTLIL::Const::iterator(it->second, _range_offset(it, addr));
	}
	// internal version of reserve_range that returns an iterator to the range
	std::map<addr_t, RTLIL::Const>::iterator _reserve_range(addr_t begin_addr, addr_t end_addr);
	// write a single word at addr, return iterator to next word
	RTLIL::Const::iterator _range_write(RTLIL::Const::iterator it, RTLIL::Const અચલ &data);
જાહેર:
	class range {
		પૂર્ણાંક _data_width;
		addr_t _base;
		RTLIL::Const અચલ &કિંમતો_2;
		friend class iterator;
		range(પૂર્ણાંક data_width, addr_t base, RTLIL::Const અચલ &કિંમતો)
		: _data_width(data_width), _base(base), કિંમતો_2(કિંમતો) {}
	જાહેર:
		addr_t base() અચલ { પાછા _base; }
		addr_t size() અચલ { પાછા ((addr_t) કિંમતો_2.size()) / _data_width; }
		addr_t limit() અચલ { પાછા _base + size(); }
		RTLIL::Const અચલ &concatenated() અચલ { પાછા કિંમતો_2; }
		RTLIL::Const operator[](addr_t addr) અચલ {
			log_assert(addr - _base < size());
			પાછા કિંમતો_2.extract((addr - _base) * _data_width, _data_width);
		}
		RTLIL::Const at_offset(addr_t offset) અચલ { પાછા (*this)[_base + offset]; }
	};
	class iterator {
		MemContents અચલ *_memory;
		// storing addr instead of an iterator gives more well-defined behaviour under insertions/deletions
		// use ~0 for end so that all end iterators compare the same
		addr_t _addr;
		friend class MemContents;
		iterator(MemContents અચલ *memory, addr_t addr) : _memory(memory), _addr(addr) {}
	જાહેર:
		using iterator_category = std::આગત_iterator_tag;
		using કિંમત_type = range;
		using pointer = arrow_proxy<range>;
		using reference = range;
		using difference_type = addr_t;
		reference operator *() અચલ { પાછા range(_memory->_data_width, _addr, _memory->કિંમતો_2.at(_addr)); }
		pointer operator->() અચલ { પાછા arrow_proxy<range>(**this); }
		bool operator !=(iterator અચલ &other) અચલ { પાછા _memory != other._memory || _addr != other._addr; }
		bool operator ==(iterator અચલ &other) અચલ { પાછા !(*this != other); }
		iterator &operator++();
	};
	MemContents(પૂર્ણાંક addr_width, પૂર્ણાંક data_width, RTLIL::Const default_કિંમત_2)
		: _addr_width(addr_width), _data_width(data_width)
		, default_કિંમત((default_કિંમત_2.extu(data_width), std::move(default_કિંમત_2)))
	{ log_assert(_addr_width > 0 && _addr_width < (પૂર્ણાંક)કદ(addr_t) * 8); log_assert(_data_width > 0); }
	MemContents(પૂર્ણાંક addr_width, પૂર્ણાંક data_width) : MemContents(addr_width, data_width, RTLIL::Const(State::Sx, data_width)) {}
	explicit MemContents(Mem *mem);
	પૂર્ણાંક addr_width() અચલ { પાછા _addr_width; }
	પૂર્ણાંક data_width() અચલ { પાછા _data_width; }
	RTLIL::Const અચલ &default_કિંમત_2() અચલ { પાછા default_કિંમત; }
	// return the value at the address if it exists, the default_value of the memory otherwise. address must not exceed 2**addr_width.
	RTLIL::Const operator [](addr_t addr) અચલ;
	// return the number of defined words in the range [begin_addr, end_addr)
	addr_t ગણતરી_range(addr_t begin_addr, addr_t end_addr) અચલ;
	// allocate memory for the range [begin_addr, end_addr), but leave the contents undefined.
	શૂન્ય reserve_range(addr_t begin_addr, addr_t end_addr) { _reserve_range(begin_addr, end_addr); }
	// insert multiple words (provided as a single concatenated RTLIL::Const) at the given address, overriding any previous assignment.
	શૂન્ય ઉમેરો_concatenated(addr_t addr, RTLIL::Const અચલ &કિંમતો);
	// insert multiple words at the given address, overriding any previous assignment.
	template<typename Iterator> શૂન્ય ઉમેરો_range(addr_t addr, Iterator begin, Iterator end) {
		auto શબ્દો_2 = end - begin;
		log_assert(addr < (addr_t)(1<<_addr_width)); log_assert(શબ્દો_2 <= (addr_t)(1<<_addr_width) - addr);
		auto range = _reserve_range(addr, addr + શબ્દો_2);
		auto it = _range_data(range, addr);
		માટે(; begin != end; ++begin)
			it = _range_write(it, *begin);
	}
	// undefine all words in the range [begin_addr, end_addr)
	શૂન્ય clear_range(addr_t begin_addr, addr_t end_addr);
	// check invariants, abort if invariants failed
	શૂન્ય check();
	iterator end() અચલ { પાછા iterator(nullptr, ~(addr_t) 0); }
	iterator begin() અચલ { પાછા કિંમતો_2.empty() ? end() : iterator(this, કિંમતો_2.begin()->first); }
	bool empty() અચલ { પાછા કિંમતો_2.empty(); }
};

YOSYS_NAMESPACE_END

#endif
