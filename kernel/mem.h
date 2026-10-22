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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/ffinit.h"
#ಅನ್ತರ್ಭಾವ "kernel/utils.h"

YOSYS_NAMESPACE_BEGIN

ಸಂರಚನಾ MemRd : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	ಪೂರ್ಣಾಙ್ಕ wide_log2;
	bool clk_enable, clk_polarity, ce_over_srst;
	Const arst_ಮೌಲ್ಯ, srst_ಮೌಲ್ಯ, init_ಮೌಲ್ಯ;
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
	SigSpec sub_addr(ಪೂರ್ಣಾಙ್ಕ sub) {
		SigSpec res = addr;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		ಪ್ರತ್ಯಾಗಮ res;
	}
};

ಸಂರಚನಾ MemWr : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	ಪೂರ್ಣಾಙ್ಕ wide_log2;
	bool clk_enable, clk_polarity;
	std::vector<bool> priority_mask;
	SigSpec clk, en, addr, data;

	MemWr() : removed(false), cell(nullptr) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(ಪೂರ್ಣಾಙ್ಕ sub) {
		SigSpec res = addr;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		ಪ್ರತ್ಯಾಗಮ res;
	}

	std::pair<SigSpec, std::vector<ಪೂರ್ಣಾಙ್ಕ>> compress_en();
	SigSpec decompress_en(ಧ್ರುವ std::vector<ಪೂರ್ಣಾಙ್ಕ> &swizzle, SigSpec sig);
};

ಸಂರಚನಾ MemInit : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	Const addr;
	Const data;
	Const en;
	MemInit() : removed(false), cell(nullptr) {}
};

ಸಂರಚನಾ Mem : RTLIL::AttrObject {
	Module *module;
	PooledName memid;
	bool packed;
	RTLIL::Memory *mem;
	Cell *cell;
	ಪೂರ್ಣಾಙ್ಕ width, start_offset, size;
	std::vector<MemInit> inits;
	std::vector<MemRd> rd_ports;
	std::vector<MemWr> wr_ports;

	// Removes this memory from the module.  The data in helper structures
	// is unaffected except for the cell/mem fields.
	ಶೂನ್ಯ remove();

	// Commits all changes in helper structures into the module — ports and
	// inits marked as removed are actually removed, new ports/inits create
	// new cells, modified port/inits are commited into their existing
	// cells.  Note that this reindexes the ports and inits array (actually
	// removing the ports/inits marked as removed).
	ಶೂನ್ಯ emit();

	// Marks all inits as removed.
	ಶೂನ್ಯ clear_inits();

	// Coalesces inits: whenever two inits have overlapping or touching
	// address ranges, they are combined into one, with the higher-priority
	// one's data overwriting the other.  Running this results in
	// an inits list equivalent to the original, in which all entries
	// cover disjoint (and non-touching) address ranges, and all enable
	// masks are all-1.
	ಶೂನ್ಯ coalesce_inits();

	// Checks consistency of this memory and all its ports/inits, using
	// log_assert.
	ಶೂನ್ಯ check();

	// Gathers all initialization data into a single big const covering
	// the whole memory.  For all non-initialized bits, Sx will be returned.
	Const get_init_data() ಧ್ರುವ;

	// Constructs and returns the helper structures for all memories
	// in a module.
	ಸ್ಥಿರ std::vector<Mem> get_all_memories(Module *module);

	// Constructs and returns the helper structures for all selected
	// memories in a module.
	ಸ್ಥಿರ std::vector<Mem> get_selected_memories(Module *module);

	// Converts a synchronous read port into an asynchronous one by
	// extracting the data (or, in some rare cases, address) register
	// into a separate cell, together with any soft-transparency
	// logic necessary to preserve its semantics.  Returns the created
	// register cell, if any.  Note that in some rare cases this function
	// may succeed and perform a conversion without creating a new
	// register — a nullptr result doesn't imply nothing was done.
	Cell *extract_rdff(ಪೂರ್ಣಾಙ್ಕ idx, FfInitVals *initvals);

	// Splits all wide ports in this memory into equivalent narrow ones.
	// This function performs no modifications at all to the actual
	// netlist unless and until emit() is called.
	ಶೂನ್ಯ narrow();

	// If write port idx2 currently has priority over write port idx1,
	// inserts extra logic on idx1's enable signal to disable writes
	// when idx2 is writing to the same address, then removes the priority
	// from the priority mask.  If there is a memory port that is
	// transparent with idx1, but not with idx2, that port is converted
	// to use soft transparency logic.
	ಶೂನ್ಯ emulate_priority(ಪೂರ್ಣಾಙ್ಕ idx1, ಪೂರ್ಣಾಙ್ಕ idx2, FfInitVals *initvals);

	// Creates soft-transparency logic on read port ridx, bypassing the
	// data from write port widx.  Should only be called when ridx is
	// transparent wrt widx in the first place.  Once we're done, the
	// transparency_mask bit will be cleared, and the collision_x_mask
	// bit will be set instead (since whatever value is read will be
	// replaced by the soft transparency logic).
	ಶೂನ್ಯ emulate_transparency(ಪೂರ್ಣಾಙ್ಕ widx, ಪೂರ್ಣಾಙ್ಕ ridx, FfInitVals *initvals);

	// Prepares for merging write port idx2 into idx1 (where idx1 < idx2).
	// Specifically, takes care of priority masks: any priority relations
	// that idx2 had are replicated onto idx1, unless they conflict with
	// priorities already present on idx1, in which case emulate_priority
	// is called.  Likewise, ensures transparency and undefined collision
	// masks of all read ports have the same values for both ports,
	// calling emulate_transparency if necessary.
	ಶೂನ್ಯ prepare_wr_merge(ಪೂರ್ಣಾಙ್ಕ idx1, ಪೂರ್ಣಾಙ್ಕ idx2, FfInitVals *initvals);

	// Prepares for merging read port idx2 into idx1.
	// Specifically, makes sure the transparency and undefined collision
	// masks of both ports are equal, by changing undefined behavior
	// of one port to the other's defined behavior, or by calling
	// emulate_transparency if necessary.
	ಶೂನ್ಯ prepare_rd_merge(ಪೂರ್ಣಾಙ್ಕ idx1, ಪೂರ್ಣಾಙ್ಕ idx2, FfInitVals *initvals);

	// Prepares the memory for widening a port to a given width.  This
	// involves ensuring that start_offset and size are aligned to the
	// target width.
	ಶೂನ್ಯ widen_prep(ಪೂರ್ಣಾಙ್ಕ wide_log2);

	// Widens a write port up to a given width.  The newly port is
	// equivalent to the original, made by replicating enable/data bits
	// and masking enable bits with decoders on the low part of the
	// original address.
	ಶೂನ್ಯ widen_wr_port(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ wide_log2);

	// Emulates a sync read port's enable functionality in soft logic,
	// changing the actual read port's enable to be always-on.
	ಶೂನ್ಯ emulate_rden(ಪೂರ್ಣಾಙ್ಕ idx, FfInitVals *initvals);

	// Emulates a sync read port's initial/reset value functionality in
	// soft logic, removing it from the actual read port.
	ಶೂನ್ಯ emulate_reset(ಪೂರ್ಣಾಙ್ಕ idx, bool emu_init, bool emu_arst, bool emu_srst, FfInitVals *initvals);

	// Given a read port with ce_over_srst set, converts it to a port
	// with ce_over_srst unset without changing its behavior by adding
	// emulation logic.
	ಶೂನ್ಯ emulate_rd_ce_over_srst(ಪೂರ್ಣಾಙ್ಕ idx);

	// Given a read port with ce_over_srst unset, converts it to a port
	// with ce_over_srst set without changing its behavior by adding
	// emulation logic.
	ಶೂನ್ಯ emulate_rd_srst_over_ce(ಪೂರ್ಣಾಙ್ಕ idx);

	// Returns true iff emulate_read_first makes sense to call.
	bool emulate_read_first_ok();

	// Emulates all read-first read-write port relationships in terms of
	// all-transparent ports, by delaying all write ports by one cycle.
	// This can only be used when all read ports and all write ports are
	// in the same clock domain.
	ಶೂನ್ಯ emulate_read_first(FfInitVals *initvals);

	Mem(Module *module, IdString memid, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ start_offset, ಪೂರ್ಣಾಙ್ಕ size) : module(module), memid(module->design, memid), packed(false), mem(nullptr), cell(nullptr), width(width), start_offset(start_offset), size(size) {}
	Mem(Module *module, TwineSpec &&memid, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ start_offset, ಪೂರ್ಣಾಙ್ಕ size)
			: Mem(module, module->twines().add(std::move(memid)), width, start_offset, size) {}
};

// MemContents efficiently represents the contents of a potentially sparse memory by storing only those segments that are actually defined
class MemContents {
ಸಾರ್ವಜನಿಕ:
	class range; class iterator;
	using addr_t = uint32_t;
ಗುಪ್ತ:
	// we ban _addr_width == sizeof(addr_t) * 8 because it adds too many cornercases
	ಪೂರ್ಣಾಙ್ಕ _addr_width;
	ಪೂರ್ಣಾಙ್ಕ _data_width;
	RTLIL::Const default_ಮೌಲ್ಯ;
	// for each range, store the concatenation of the words at the start address
	// invariants:
	// - no overlapping or adjacent ranges
	// - no empty ranges
	// - all Consts are a multiple of the word size
	std::map<addr_t, RTLIL::Const> ಮೌಲ್ಯಗಳು_2;
	// returns an iterator to the range containing addr, if it exists, or the first range past addr
	std::map<addr_t, RTLIL::Const>::iterator _range_at(addr_t addr) ಧ್ರುವ;
	addr_t _range_size(std::map<addr_t, RTLIL::Const>::iterator it) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ it->second.size() / _data_width; }
	addr_t _range_begin(std::map<addr_t, RTLIL::Const>::iterator it) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ it->first; }
	addr_t _range_end(std::map<addr_t, RTLIL::Const>::iterator it) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _range_begin(it) + _range_size(it); }
	// check if the iterator points to a range containing addr
	bool _range_contains(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ಧ್ರುವ;
	// check if the iterator points to a range containing [begin_addr, end_addr). assumes end_addr >= begin_addr.
	bool _range_contains(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t end_addr) ಧ್ರುವ;
	// check if the iterator points to a range overlapping with [begin_addr, end_addr)
	bool _range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t end_addr) ಧ್ರುವ;
	// return the offset the addr would have in the range at `it`
	size_t _range_offset(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (addr - it->first) * _data_width; }
	// assuming _range_contains(it, addr), return an iterator pointing to the data at addr
	RTLIL::Const::iterator _range_data(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) {
		ಪ್ರತ್ಯಾಗಮ RTLIL::Const::iterator(it->second, _range_offset(it, addr));
	}
	// internal version of reserve_range that returns an iterator to the range
	std::map<addr_t, RTLIL::Const>::iterator _reserve_range(addr_t begin_addr, addr_t end_addr);
	// write a single word at addr, return iterator to next word
	RTLIL::Const::iterator _range_write(RTLIL::Const::iterator it, RTLIL::Const ಧ್ರುವ &data);
ಸಾರ್ವಜನಿಕ:
	class range {
		ಪೂರ್ಣಾಙ್ಕ _data_width;
		addr_t _base;
		RTLIL::Const ಧ್ರುವ &ಮೌಲ್ಯಗಳು_2;
		friend class iterator;
		range(ಪೂರ್ಣಾಙ್ಕ data_width, addr_t base, RTLIL::Const ಧ್ರುವ &ಮೌಲ್ಯಗಳು)
		: _data_width(data_width), _base(base), ಮೌಲ್ಯಗಳು_2(ಮೌಲ್ಯಗಳು) {}
	ಸಾರ್ವಜನಿಕ:
		addr_t base() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _base; }
		addr_t size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ((addr_t) ಮೌಲ್ಯಗಳು_2.size()) / _data_width; }
		addr_t limit() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _base + size(); }
		RTLIL::Const ಧ್ರುವ &concatenated() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯಗಳು_2; }
		RTLIL::Const operator[](addr_t addr) ಧ್ರುವ {
			log_assert(addr - _base < size());
			ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯಗಳು_2.extract((addr - _base) * _data_width, _data_width);
		}
		RTLIL::Const at_offset(addr_t offset) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (*this)[_base + offset]; }
	};
	class iterator {
		MemContents ಧ್ರುವ *_memory;
		// storing addr instead of an iterator gives more well-defined behaviour under insertions/deletions
		// use ~0 for end so that all end iterators compare the same
		addr_t _addr;
		friend class MemContents;
		iterator(MemContents ಧ್ರುವ *memory, addr_t addr) : _memory(memory), _addr(addr) {}
	ಸಾರ್ವಜನಿಕ:
		using iterator_category = std::ಒಳಹರಿವು_iterator_tag;
		using ಮೌಲ್ಯ_type = range;
		using pointer = arrow_proxy<range>;
		using reference = range;
		using difference_type = addr_t;
		reference operator *() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ range(_memory->_data_width, _addr, _memory->ಮೌಲ್ಯಗಳು_2.at(_addr)); }
		pointer operator->() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ arrow_proxy<range>(**this); }
		bool operator !=(iterator ಧ್ರುವ &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _memory != other._memory || _addr != other._addr; }
		bool operator ==(iterator ಧ್ರುವ &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ !(*this != other); }
		iterator &operator++();
	};
	MemContents(ಪೂರ್ಣಾಙ್ಕ addr_width, ಪೂರ್ಣಾಙ್ಕ data_width, RTLIL::Const default_ಮೌಲ್ಯ_2)
		: _addr_width(addr_width), _data_width(data_width)
		, default_ಮೌಲ್ಯ((default_ಮೌಲ್ಯ_2.extu(data_width), std::move(default_ಮೌಲ್ಯ_2)))
	{ log_assert(_addr_width > 0 && _addr_width < (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(addr_t) * 8); log_assert(_data_width > 0); }
	MemContents(ಪೂರ್ಣಾಙ್ಕ addr_width, ಪೂರ್ಣಾಙ್ಕ data_width) : MemContents(addr_width, data_width, RTLIL::Const(State::Sx, data_width)) {}
	explicit MemContents(Mem *mem);
	ಪೂರ್ಣಾಙ್ಕ addr_width() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _addr_width; }
	ಪೂರ್ಣಾಙ್ಕ data_width() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _data_width; }
	RTLIL::Const ಧ್ರುವ &default_ಮೌಲ್ಯ_2() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ default_ಮೌಲ್ಯ; }
	// return the value at the address if it exists, the default_value of the memory otherwise. address must not exceed 2**addr_width.
	RTLIL::Const operator [](addr_t addr) ಧ್ರುವ;
	// return the number of defined words in the range [begin_addr, end_addr)
	addr_t ಎಣಿಕೆ_range(addr_t begin_addr, addr_t end_addr) ಧ್ರುವ;
	// allocate memory for the range [begin_addr, end_addr), but leave the contents undefined.
	ಶೂನ್ಯ reserve_range(addr_t begin_addr, addr_t end_addr) { _reserve_range(begin_addr, end_addr); }
	// insert multiple words (provided as a single concatenated RTLIL::Const) at the given address, overriding any previous assignment.
	ಶೂನ್ಯ ಸೇರಿಸು_concatenated(addr_t addr, RTLIL::Const ಧ್ರುವ &ಮೌಲ್ಯಗಳು);
	// insert multiple words at the given address, overriding any previous assignment.
	template<typename Iterator> ಶೂನ್ಯ ಸೇರಿಸು_range(addr_t addr, Iterator begin, Iterator end) {
		auto ಪದಗಳು_2 = end - begin;
		log_assert(addr < (addr_t)(1<<_addr_width)); log_assert(ಪದಗಳು_2 <= (addr_t)(1<<_addr_width) - addr);
		auto range = _reserve_range(addr, addr + ಪದಗಳು_2);
		auto it = _range_data(range, addr);
		ಪ್ರತಿ(; begin != end; ++begin)
			it = _range_write(it, *begin);
	}
	// undefine all words in the range [begin_addr, end_addr)
	ಶೂನ್ಯ clear_range(addr_t begin_addr, addr_t end_addr);
	// check invariants, abort if invariants failed
	ಶೂನ್ಯ check();
	iterator end() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ iterator(nullptr, ~(addr_t) 0); }
	iterator begin() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯಗಳು_2.empty() ? end() : iterator(this, ಮೌಲ್ಯಗಳು_2.begin()->first); }
	bool empty() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯಗಳು_2.empty(); }
};

YOSYS_NAMESPACE_END

#endif
