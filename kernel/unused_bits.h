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
		لكل (auto cell : module->cells())
			لكل (auto &conn : cell->connections())
				إذا (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		لكل (auto wire : module->wires())
			إذا (wire->port_output)
				add_used(wire);
	}

	فراغ add_used(ثابت RTLIL::SigSpec &sig)
	{
		لكل (auto bit : sigmap(sig))
			إذا (bit.wire != nullptr)
				used.أدرج(bit);
	}

	bool check(RTLIL::SigBit bit) ثابت
	{
		RTLIL::SigBit mapped = sigmap(bit);
		أرجع mapped.wire != nullptr && !used.عدد(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
