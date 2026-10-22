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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MEM_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MEM_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/ffinit.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/utils.h"

YOSYS_NAMESPACE_BEGIN

ਬਣਤਰ MemRd : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	ਪੂਰਨ wide_log2;
	bool clk_enable, clk_polarity, ce_over_srst;
	Const arst_ਮੁੱਲ, srst_ਮੁੱਲ, init_ਮੁੱਲ;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will bypass the written data
	// to this port's output (default behavior is to read old value).
	// Can only be set for write ports that have the same clock domain.
	std::ਸਦਿਸ਼<bool> transparency_ਨਕਾਬ;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will return an all-X (don't care)
	// value.  Mutually exclusive with transparency_mask.
	// Can only be set for write ports that have the same clock domain.
	// For optimization purposes, this will also be set if we can
	// determine that the two ports can never be active simultanously
	// (making the above vacuously true).
	std::ਸਦਿਸ਼<bool> collision_x_ਨਕਾਬ;
	SigSpec clk, en, arst, srst, addr, ਅੰਕੜੇ;

	MemRd() : removed(false), cell(nullptr), wide_log2(0), clk_enable(false), clk_polarity(true), ce_over_srst(false), clk(State::Sx), en(State::S1), arst(State::S0), srst(State::S0) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(ਪੂਰਨ sub) {
		SigSpec res = addr;
		ਲਈ (ਪੂਰਨ i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		ਵਾਪਸ res;
	}
};

ਬਣਤਰ MemWr : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	ਪੂਰਨ wide_log2;
	bool clk_enable, clk_polarity;
	std::ਸਦਿਸ਼<bool> ਤਰਜੀਹ_ਨਕਾਬ;
	SigSpec clk, en, addr, ਅੰਕੜੇ;

	MemWr() : removed(false), cell(nullptr) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(ਪੂਰਨ sub) {
		SigSpec res = addr;
		ਲਈ (ਪੂਰਨ i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		ਵਾਪਸ res;
	}

	std::pair<SigSpec, std::ਸਦਿਸ਼<ਪੂਰਨ>> compress_en();
	SigSpec decompress_en(ਅਚਲ std::ਸਦਿਸ਼<ਪੂਰਨ> &swizzle, SigSpec sig);
};

ਬਣਤਰ MemInit : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	Const addr;
	Const ਅੰਕੜੇ;
	Const en;
	MemInit() : removed(false), cell(nullptr) {}
};

ਬਣਤਰ Mem : RTLIL::AttrObject {
	Module *module;
	PooledName memid;
	bool packed;
	RTLIL::ਸਮ੍ਰਿਤੀ *mem;
	Cell *cell;
	ਪੂਰਨ ਚੌੜਾਈ, ਸ਼ੁਰੂ_ਵਿੱਥ, ਮਾਪ;
	std::ਸਦਿਸ਼<MemInit> inits;
	std::ਸਦਿਸ਼<MemRd> rd_ports;
	std::ਸਦਿਸ਼<MemWr> wr_ports;

	// Removes this memory from the module.  The data in helper structures
	// is unaffected except for the cell/mem fields.
	ਖਾਲੀ remove();

	// Commits all changes in helper structures into the module — ports and
	// inits marked as removed are actually removed, new ports/inits create
	// new cells, modified port/inits are commited into their existing
	// cells.  Note that this reindexes the ports and inits array (actually
	// removing the ports/inits marked as removed).
	ਖਾਲੀ emit();

	// Marks all inits as removed.
	ਖਾਲੀ clear_inits();

	// Coalesces inits: whenever two inits have overlapping or touching
	// address ranges, they are combined into one, with the higher-priority
	// one's data overwriting the other.  Running this results in
	// an inits list equivalent to the original, in which all entries
	// cover disjoint (and non-touching) address ranges, and all enable
	// masks are all-1.
	ਖਾਲੀ coalesce_inits();

	// Checks consistency of this memory and all its ports/inits, using
	// log_assert.
	ਖਾਲੀ check();

	// Gathers all initialization data into a single big const covering
	// the whole memory.  For all non-initialized bits, Sx will be returned.
	Const get_init_ਅੰਕੜੇ() ਅਚਲ;

	// Constructs and returns the helper structures for all memories
	// in a module.
	ਸਥਿਰ std::ਸਦਿਸ਼<Mem> get_all_memories(Module *module);

	// Constructs and returns the helper structures for all selected
	// memories in a module.
	ਸਥਿਰ std::ਸਦਿਸ਼<Mem> get_selected_memories(Module *module);

	// Converts a synchronous read port into an asynchronous one by
	// extracting the data (or, in some rare cases, address) register
	// into a separate cell, together with any soft-transparency
	// logic necessary to preserve its semantics.  Returns the created
	// register cell, if any.  Note that in some rare cases this function
	// may succeed and perform a conversion without creating a new
	// register — a nullptr result doesn't imply nothing was done.
	Cell *ਕੱਢੋ_rdff(ਪੂਰਨ idx, FfInitVals *initvals);

	// Splits all wide ports in this memory into equivalent narrow ones.
	// This function performs no modifications at all to the actual
	// netlist unless and until emit() is called.
	ਖਾਲੀ narrow();

	// If write port idx2 currently has priority over write port idx1,
	// inserts extra logic on idx1's enable signal to disable writes
	// when idx2 is writing to the same address, then removes the priority
	// from the priority mask.  If there is a memory port that is
	// transparent with idx1, but not with idx2, that port is converted
	// to use soft transparency logic.
	ਖਾਲੀ emulate_ਤਰਜੀਹ(ਪੂਰਨ idx1, ਪੂਰਨ idx2, FfInitVals *initvals);

	// Creates soft-transparency logic on read port ridx, bypassing the
	// data from write port widx.  Should only be called when ridx is
	// transparent wrt widx in the first place.  Once we're done, the
	// transparency_mask bit will be cleared, and the collision_x_mask
	// bit will be set instead (since whatever value is read will be
	// replaced by the soft transparency logic).
	ਖਾਲੀ emulate_transparency(ਪੂਰਨ widx, ਪੂਰਨ ridx, FfInitVals *initvals);

	// Prepares for merging write port idx2 into idx1 (where idx1 < idx2).
	// Specifically, takes care of priority masks: any priority relations
	// that idx2 had are replicated onto idx1, unless they conflict with
	// priorities already present on idx1, in which case emulate_priority
	// is called.  Likewise, ensures transparency and undefined collision
	// masks of all read ports have the same values for both ports,
	// calling emulate_transparency if necessary.
	ਖਾਲੀ prepare_wr_ਮਿਲਾਓ(ਪੂਰਨ idx1, ਪੂਰਨ idx2, FfInitVals *initvals);

	// Prepares for merging read port idx2 into idx1.
	// Specifically, makes sure the transparency and undefined collision
	// masks of both ports are equal, by changing undefined behavior
	// of one port to the other's defined behavior, or by calling
	// emulate_transparency if necessary.
	ਖਾਲੀ prepare_rd_ਮਿਲਾਓ(ਪੂਰਨ idx1, ਪੂਰਨ idx2, FfInitVals *initvals);

	// Prepares the memory for widening a port to a given width.  This
	// involves ensuring that start_offset and size are aligned to the
	// target width.
	ਖਾਲੀ widen_prep(ਪੂਰਨ wide_log2);

	// Widens a write port up to a given width.  The newly port is
	// equivalent to the original, made by replicating enable/data bits
	// and masking enable bits with decoders on the low part of the
	// original address.
	ਖਾਲੀ widen_wr_ਪੋਰਟ(ਪੂਰਨ idx, ਪੂਰਨ wide_log2);

	// Emulates a sync read port's enable functionality in soft logic,
	// changing the actual read port's enable to be always-on.
	ਖਾਲੀ emulate_rden(ਪੂਰਨ idx, FfInitVals *initvals);

	// Emulates a sync read port's initial/reset value functionality in
	// soft logic, removing it from the actual read port.
	ਖਾਲੀ emulate_reset(ਪੂਰਨ idx, bool emu_init, bool emu_arst, bool emu_srst, FfInitVals *initvals);

	// Given a read port with ce_over_srst set, converts it to a port
	// with ce_over_srst unset without changing its behavior by adding
	// emulation logic.
	ਖਾਲੀ emulate_rd_ce_over_srst(ਪੂਰਨ idx);

	// Given a read port with ce_over_srst unset, converts it to a port
	// with ce_over_srst set without changing its behavior by adding
	// emulation logic.
	ਖਾਲੀ emulate_rd_srst_over_ce(ਪੂਰਨ idx);

	// Returns true iff emulate_read_first makes sense to call.
	bool emulate_read_ਪਹਿਲਾ_ok();

	// Emulates all read-first read-write port relationships in terms of
	// all-transparent ports, by delaying all write ports by one cycle.
	// This can only be used when all read ports and all write ports are
	// in the same clock domain.
	ਖਾਲੀ emulate_read_ਪਹਿਲਾ(FfInitVals *initvals);

	Mem(Module *module, IdString memid, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਸ਼ੁਰੂ_ਵਿੱਥ, ਪੂਰਨ ਮਾਪ) : module(module), memid(module->design, memid), packed(false), mem(nullptr), cell(nullptr), ਚੌੜਾਈ(ਚੌੜਾਈ), ਸ਼ੁਰੂ_ਵਿੱਥ(ਸ਼ੁਰੂ_ਵਿੱਥ), ਮਾਪ(ਮਾਪ) {}
	Mem(Module *module, TwineSpec &&memid, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਸ਼ੁਰੂ_ਵਿੱਥ, ਪੂਰਨ ਮਾਪ)
			: Mem(module, module->twines().ਜੋੜੋ(std::move(memid)), ਚੌੜਾਈ, ਸ਼ੁਰੂ_ਵਿੱਥ, ਮਾਪ) {}
};

// MemContents efficiently represents the contents of a potentially sparse memory by storing only those segments that are actually defined
class MemContents {
ਜਨਤਕ:
	class range; class iterator;
	using addr_t = uint32_t;
ਨਿਜੀ:
	// we ban _addr_width == sizeof(addr_t) * 8 because it adds too many cornercases
	ਪੂਰਨ addr_ਚੌੜਾਈ_2;
	ਪੂਰਨ ਅੰਕੜੇ_ਚੌੜਾਈ_2;
	RTLIL::Const default_ਮੁੱਲ;
	// for each range, store the concatenation of the words at the start address
	// invariants:
	// - no overlapping or adjacent ranges
	// - no empty ranges
	// - all Consts are a multiple of the word size
	std::map<addr_t, RTLIL::Const> ਮੁੱਲਾਂ_2;
	// returns an iterator to the range containing addr, if it exists, or the first range past addr
	std::map<addr_t, RTLIL::Const>::iterator _range_at(addr_t addr) ਅਚਲ;
	addr_t range_ਮਾਪ(std::map<addr_t, RTLIL::Const>::iterator it) ਅਚਲ { ਵਾਪਸ it->ਦੂਜਾ.ਮਾਪ() / ਅੰਕੜੇ_ਚੌੜਾਈ_2; }
	addr_t _range_begin(std::map<addr_t, RTLIL::Const>::iterator it) ਅਚਲ { ਵਾਪਸ it->ਪਹਿਲਾ; }
	addr_t range_ਸਿਰਾ(std::map<addr_t, RTLIL::Const>::iterator it) ਅਚਲ { ਵਾਪਸ _range_begin(it) + range_ਮਾਪ(it); }
	// check if the iterator points to a range containing addr
	bool range_ਸ਼ਾਮਲ_ਹੈ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ਅਚਲ;
	// check if the iterator points to a range containing [begin_addr, end_addr). assumes end_addr >= begin_addr.
	bool range_ਸ਼ਾਮਲ_ਹੈ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t ਸਿਰਾ_addr) ਅਚਲ;
	// check if the iterator points to a range overlapping with [begin_addr, end_addr)
	bool _range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t ਸਿਰਾ_addr) ਅਚਲ;
	// return the offset the addr would have in the range at `it`
	ਮਾਪ_t range_ਵਿੱਥ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ਅਚਲ { ਵਾਪਸ (addr - it->ਪਹਿਲਾ) * ਅੰਕੜੇ_ਚੌੜਾਈ_2; }
	// assuming _range_contains(it, addr), return an iterator pointing to the data at addr
	RTLIL::Const::iterator range_ਅੰਕੜੇ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) {
		ਵਾਪਸ RTLIL::Const::iterator(it->ਦੂਜਾ, range_ਵਿੱਥ(it, addr));
	}
	// internal version of reserve_range that returns an iterator to the range
	std::map<addr_t, RTLIL::Const>::iterator _reserve_range(addr_t begin_addr, addr_t ਸਿਰਾ_addr);
	// write a single word at addr, return iterator to next word
	RTLIL::Const::iterator _range_write(RTLIL::Const::iterator it, RTLIL::Const ਅਚਲ &ਅੰਕੜੇ);
ਜਨਤਕ:
	class range {
		ਪੂਰਨ ਅੰਕੜੇ_ਚੌੜਾਈ_2;
		addr_t ਅਧਾਰ_2;
		RTLIL::Const ਅਚਲ &ਮੁੱਲਾਂ_2;
		ਮਿੱਤਰ class iterator;
		range(ਪੂਰਨ ਅੰਕੜੇ_ਚੌੜਾਈ, addr_t ਅਧਾਰ, RTLIL::Const ਅਚਲ &ਮੁੱਲਾਂ)
		: ਅੰਕੜੇ_ਚੌੜਾਈ_2(ਅੰਕੜੇ_ਚੌੜਾਈ), ਅਧਾਰ_2(ਅਧਾਰ), ਮੁੱਲਾਂ_2(ਮੁੱਲਾਂ) {}
	ਜਨਤਕ:
		addr_t ਅਧਾਰ() ਅਚਲ { ਵਾਪਸ ਅਧਾਰ_2; }
		addr_t ਮਾਪ() ਅਚਲ { ਵਾਪਸ ((addr_t) ਮੁੱਲਾਂ_2.ਮਾਪ()) / ਅੰਕੜੇ_ਚੌੜਾਈ_2; }
		addr_t ਹੱਦਬੰਦੀ() ਅਚਲ { ਵਾਪਸ ਅਧਾਰ_2 + ਮਾਪ(); }
		RTLIL::Const ਅਚਲ &concatenated() ਅਚਲ { ਵਾਪਸ ਮੁੱਲਾਂ_2; }
		RTLIL::Const ਸੰਚਾਲਕ[](addr_t addr) ਅਚਲ {
			log_assert(addr - ਅਧਾਰ_2 < ਮਾਪ());
			ਵਾਪਸ ਮੁੱਲਾਂ_2.ਕੱਢੋ_2((addr - ਅਧਾਰ_2) * ਅੰਕੜੇ_ਚੌੜਾਈ_2, ਅੰਕੜੇ_ਚੌੜਾਈ_2);
		}
		RTLIL::Const at_ਵਿੱਥ(addr_t ਵਿੱਥ) ਅਚਲ { ਵਾਪਸ (*this)[ਅਧਾਰ_2 + ਵਿੱਥ]; }
	};
	class iterator {
		MemContents ਅਚਲ *ਸਮ੍ਰਿਤੀ_2;
		// storing addr instead of an iterator gives more well-defined behaviour under insertions/deletions
		// use ~0 for end so that all end iterators compare the same
		addr_t _addr;
		ਮਿੱਤਰ class MemContents;
		iterator(MemContents ਅਚਲ *ਸਮ੍ਰਿਤੀ_3, addr_t addr) : ਸਮ੍ਰਿਤੀ_2(ਸਮ੍ਰਿਤੀ_3), _addr(addr) {}
	ਜਨਤਕ:
		using iterator_category = std::ਨਿਵੇਸ਼_iterator_ਟੈਗ;
		using ਮੁੱਲ_type = range;
		using ਸੂਚਕ = arrow_proxy<range>;
		using reference = range;
		using difference_type = addr_t;
		reference ਸੰਚਾਲਕ *() ਅਚਲ { ਵਾਪਸ range(ਸਮ੍ਰਿਤੀ_2->ਅੰਕੜੇ_ਚੌੜਾਈ_2, _addr, ਸਮ੍ਰਿਤੀ_2->ਮੁੱਲਾਂ_2.at(_addr)); }
		ਸੂਚਕ ਸੰਚਾਲਕ->() ਅਚਲ { ਵਾਪਸ arrow_proxy<range>(**this); }
		bool ਸੰਚਾਲਕ !=(iterator ਅਚਲ &other) ਅਚਲ { ਵਾਪਸ ਸਮ੍ਰਿਤੀ_2 != other.ਸਮ੍ਰਿਤੀ_2 || _addr != other._addr; }
		bool ਸੰਚਾਲਕ ==(iterator ਅਚਲ &other) ਅਚਲ { ਵਾਪਸ !(*this != other); }
		iterator &ਸੰਚਾਲਕ++();
	};
	MemContents(ਪੂਰਨ addr_ਚੌੜਾਈ, ਪੂਰਨ ਅੰਕੜੇ_ਚੌੜਾਈ, RTLIL::Const default_ਮੁੱਲ_2)
		: addr_ਚੌੜਾਈ_2(addr_ਚੌੜਾਈ), ਅੰਕੜੇ_ਚੌੜਾਈ_2(ਅੰਕੜੇ_ਚੌੜਾਈ)
		, default_ਮੁੱਲ((default_ਮੁੱਲ_2.extu(ਅੰਕੜੇ_ਚੌੜਾਈ), std::move(default_ਮੁੱਲ_2)))
	{ log_assert(addr_ਚੌੜਾਈ_2 > 0 && addr_ਚੌੜਾਈ_2 < (ਪੂਰਨ)ਆਕਾਰ(addr_t) * 8); log_assert(ਅੰਕੜੇ_ਚੌੜਾਈ_2 > 0); }
	MemContents(ਪੂਰਨ addr_ਚੌੜਾਈ, ਪੂਰਨ ਅੰਕੜੇ_ਚੌੜਾਈ) : MemContents(addr_ਚੌੜਾਈ, ਅੰਕੜੇ_ਚੌੜਾਈ, RTLIL::Const(State::Sx, ਅੰਕੜੇ_ਚੌੜਾਈ)) {}
	explicit MemContents(Mem *mem);
	ਪੂਰਨ addr_ਚੌੜਾਈ() ਅਚਲ { ਵਾਪਸ addr_ਚੌੜਾਈ_2; }
	ਪੂਰਨ ਅੰਕੜੇ_ਚੌੜਾਈ() ਅਚਲ { ਵਾਪਸ ਅੰਕੜੇ_ਚੌੜਾਈ_2; }
	RTLIL::Const ਅਚਲ &default_ਮੁੱਲ_2() ਅਚਲ { ਵਾਪਸ default_ਮੁੱਲ; }
	// return the value at the address if it exists, the default_value of the memory otherwise. address must not exceed 2**addr_width.
	RTLIL::Const ਸੰਚਾਲਕ [](addr_t addr) ਅਚਲ;
	// return the number of defined words in the range [begin_addr, end_addr)
	addr_t ਗਿਣਤੀ_range(addr_t begin_addr, addr_t ਸਿਰਾ_addr) ਅਚਲ;
	// allocate memory for the range [begin_addr, end_addr), but leave the contents undefined.
	ਖਾਲੀ reserve_range(addr_t begin_addr, addr_t ਸਿਰਾ_addr) { _reserve_range(begin_addr, ਸਿਰਾ_addr); }
	// insert multiple words (provided as a single concatenated RTLIL::Const) at the given address, overriding any previous assignment.
	ਖਾਲੀ ਪਾਓ_concatenated(addr_t addr, RTLIL::Const ਅਚਲ &ਮੁੱਲਾਂ);
	// insert multiple words at the given address, overriding any previous assignment.
	ਸਾਂਚਾ<typename Iterator> ਖਾਲੀ ਪਾਓ_range(addr_t addr, Iterator begin, Iterator ਸਿਰਾ) {
		ਸਵੈਚਾਲਿਤ ਸ਼ਬਦ_3 = ਸਿਰਾ - begin;
		log_assert(addr < (addr_t)(1<<addr_ਚੌੜਾਈ_2)); log_assert(ਸ਼ਬਦ_3 <= (addr_t)(1<<addr_ਚੌੜਾਈ_2) - addr);
		ਸਵੈਚਾਲਿਤ range = _reserve_range(addr, addr + ਸ਼ਬਦ_3);
		ਸਵੈਚਾਲਿਤ it = range_ਅੰਕੜੇ(range, addr);
		ਲਈ(; begin != ਸਿਰਾ; ++begin)
			it = _range_write(it, *begin);
	}
	// undefine all words in the range [begin_addr, end_addr)
	ਖਾਲੀ clear_range(addr_t begin_addr, addr_t ਸਿਰਾ_addr);
	// check invariants, abort if invariants failed
	ਖਾਲੀ check();
	iterator ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ iterator(nullptr, ~(addr_t) 0); }
	iterator begin() ਅਚਲ { ਵਾਪਸ ਮੁੱਲਾਂ_2.ਖਾਲੀ_ਥਾਵਾਂ() ? ਸਿਰਾ() : iterator(this, ਮੁੱਲਾਂ_2.begin()->ਪਹਿਲਾ); }
	bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ ਮੁੱਲਾਂ_2.ਖਾਲੀ_ਥਾਵਾਂ(); }
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
