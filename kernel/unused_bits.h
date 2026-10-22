#ifndef UNUSED_BITS_H
#define UNUSED_BITS_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

بنية UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, ثابت NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	فراغ collect_used(RTLIL::Module *module, ثابت NewCellTypes &ct)
	{
		עבור (auto cell : module->cells())
			עבור (auto &conn : cell->connections())
				אם (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		עבור (auto wire : module->wires())
			אם (wire->port_output)
				add_used(wire);
	}

	فراغ add_used(ثابت RTLIL::SigSpec &sig)
	{
		עבור (auto bit : sigmap(sig))
			אם (bit.wire != nullptr)
				used.insert(bit);
	}

	bool check(RTLIL::SigBit bit) ثابت
	{
		RTLIL::SigBit mapped = sigmap(bit);
		החזר mapped.wire != nullptr && !used.count(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
