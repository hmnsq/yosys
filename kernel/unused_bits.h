#ifndef UNUSED_BITS_H
#define UNUSED_BITS_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ಸಂರಚನಾ UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, ಧ್ರುವ NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	ಶೂನ್ಯ collect_used(RTLIL::Module *module, ಧ್ರುವ NewCellTypes &ct)
	{
		ಪ್ರತಿ (auto cell : module->cells())
			ಪ್ರತಿ (auto &conn : cell->connections())
				ಯದಿ (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		ಪ್ರತಿ (auto wire : module->wires())
			ಯದಿ (wire->port_output)
				add_used(wire);
	}

	ಶೂನ್ಯ add_used(ಧ್ರುವ RTLIL::SigSpec &sig)
	{
		ಪ್ರತಿ (auto bit : sigmap(sig))
			ಯದಿ (bit.wire != nullptr)
				used.ಸೇರಿಸು(bit);
	}

	bool check(RTLIL::SigBit bit) ಧ್ರುವ
	{
		RTLIL::SigBit mapped = sigmap(bit);
		ಪ್ರತ್ಯಾಗಮ mapped.wire != nullptr && !used.ಎಣಿಕೆ(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
