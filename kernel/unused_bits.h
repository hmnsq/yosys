#ifndef UNUSED_BITS_H
#define UNUSED_BITS_H

#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

struct UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, const NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	void collect_used(RTLIL::Module *module, const NewCellTypes &ct)
	{
		위해 (auto cell : module->cells())
			위해 (auto &conn : cell->connections())
				만약 (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		위해 (auto wire : module->wires())
			만약 (wire->port_output)
				add_used(wire);
	}

	void add_used(const RTLIL::SigSpec &sig)
	{
		위해 (auto bit : sigmap(sig))
			만약 (bit.wire != nullptr)
				used.insert(bit);
	}

	bool check(RTLIL::SigBit bit) const
	{
		RTLIL::SigBit mapped = sigmap(bit);
		반환 mapped.wire != nullptr && !used.count(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
