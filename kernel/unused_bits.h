#جيڪڏهن_وصف_نه_ٿيل اڻ_استعمال_ٿيل_بٽ_H
#وصف_ڪريو اڻ_استعمال_ٿيل_بٽ_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

بنية اڻ_استعمال_ٿيل_بٽ
{
	SigMap sigmap;
	pool<RTLIL::Sig_ٻٽو_انگ> استعمال_ٿيل;

	اڻ_استعمال_ٿيل_بٽ(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_استعمال_ٿيل(module, ct);
	}

	اڻ_استعمال_ٿيل_بٽ(RTLIL::Module *module, ثابت NewCellTypes &ct) : sigmap(module)
	{
		collect_استعمال_ٿيل(module, ct);
	}

	فراغ collect_استعمال_ٿيل(RTLIL::Module *module, ثابت NewCellTypes &ct)
	{
		لكل (خودڪار cell : module->cells())
			لكل (خودڪار &conn : cell->connections())
				إذا (!ct.cell_output(cell->type, conn.پهريون))
					جوڙيو_استعمال_ٿيل(conn.ٻيو);

		لكل (خودڪار wire : module->wires())
			إذا (wire->پورٽ_output)
				جوڙيو_استعمال_ٿيل(wire);
	}

	فراغ جوڙيو_استعمال_ٿيل(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودڪار ٻٽو_انگ : sigmap(sig))
			إذا (ٻٽو_انگ.wire != nullptr)
				استعمال_ٿيل.وجهو(ٻٽو_انگ);
	}

	bool check(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ) ثابت
	{
		RTLIL::Sig_ٻٽو_انگ mapped = sigmap(ٻٽو_انگ);
		أرجع mapped.wire != nullptr && !استعمال_ٿيل.ڳڻپ(mapped);
	}
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
