#ضمن "kernel/twine.h"
#ضمن "kernel/hashcons_impl.h"
#ضمن "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

قالب بنية HashConsPool<TwinePool, Twine_گره, IdString>;

std::وکتور<Twine_گره> StaticTwines::nodes_;

فراغ StaticTwines::init() {
	إذا (ready())
		أرجع;
	log_assert(nodes_.خالی());
	nodes_.reserve(STATIC_TWINE_انتها);
	لكل (ثابت حرف *name : ID::static_نامها)
		nodes_.emplace_پشت(std::string_view(name));
}

ثابت Twine_گره &StaticTwines::گره(اندازه_t idx) { أرجع nodes_[idx]; }
منطقي StaticTwines::ready() { أرجع nodes_.اندازه() == STATIC_TWINE_انتها; }

int64_t twine_gc_ns;
صحيح twine_gc_شمار;

std::string IdString::handle_token() ثابت {
	أرجع stringf("%s@%zu", is_عمومی() ? "$pub" : "$priv", untag().raw());
}

اندازه_t IdString::handle_token_پیشوند(std::string_view token, منطقي &is_عمومی_2) {
	إذا (token.substr(0, 5) == "$pub@") {
		is_عمومی_2 = درست;
		أرجع 5;
	}
	إذا (token.substr(0, 6) == "$priv@") {
		is_عمومی_2 = خطأ;
		أرجع 6;
	}
	أرجع 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_انتها);
	std::string نتیجه = ref.is_عمومی() ? "\\" : "";
	نتیجه += static_نامها[idx.raw()];
	أرجع نتیجه;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_انتها);
	أرجع static_نامها[idx.raw()];
}

TwineSpec::TwineSpec(برگ v) : داده(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : داده(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : داده(std::move(v)) {}

فراغ SmallString::store(std::string_view content)
{
	log_assert(content.اندازه() <= MAX_LEN);
	release();
	len_ = content.اندازه();
	إذا (len_ > INLINE_CAP) {
		ptr_ = جديد حرف[len_];
		کپی_حافظه(ptr_, content.داده(), len_);
	} وإلا {
		کپی_حافظه(inl_, content.داده(), len_);
	}
}

فراغ SmallString::release()
{
	إذا (len_ > INLINE_CAP)
		حذف[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ثابت SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	کپی_حافظه(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::عملگر=(ثابت SmallString &other)
{
	إذا (هذا != &other)
		store(other.view());
	أرجع *هذا;
}

SmallString &SmallString::عملگر=(SmallString &&other) noexcept
{
	إذا (هذا == &other)
		أرجع *هذا;
	release();
	کپی_حافظه(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	أرجع *هذا;
}

SmallString::~SmallString() { release(); }

Twine_گره::Twine_گره(Twine_گره &&other) noexcept
	: متن_2(std::move(other.متن_2)), پیشوند_3(other.پیشوند_3), hash_(other.hash_)
{
	other.پیشوند_3 = DEAD;
}

Twine_گره &Twine_گره::عملگر=(Twine_گره &&other) noexcept
{
	إذا (هذا == &other)
		أرجع *هذا;
	متن_2 = std::move(other.متن_2);
	پیشوند_3 = other.پیشوند_3;
	hash_ = other.hash_;
	other.پیشوند_3 = DEAD;
	أرجع *هذا;
}

std::string TwineSpec::content_str() ثابت {
	إذا (خودکار *برگ_2 = std::get_if<برگ>(&داده))
		أرجع برگ_2->s;
	خودکار &autosfx = std::get<AutoSuffix>(داده);
	أرجع *autosfx.پیشوند_2 + autosfx.دم;
}

ساكن std::pair<std::string_view, منطقي> unescape_name(std::string_view name) {
	إذا (!name.خالی() && name[0] == '\\')
		أرجع {name.substr(1), درست};
	أرجع {name, خطأ};
}

ساكن فراغ check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	لكل (حرف ch : name)
		إذا ((بیعلامت)ch <= (بیعلامت)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(بعدی_serial()) {}
TwinePool::TwinePool(ثابت TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(بعدی_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(بعدی_serial()) {}

TwinePool& TwinePool::عملگر=(ثابت TwinePool& other) {
	إذا (هذا == &other)
		أرجع *هذا;
	HashConsPool::عملگر=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = بعدی_serial();
	أرجع *هذا;
}

TwinePool& TwinePool::عملگر=(TwinePool&& other) {
	إذا (هذا == &other)
		أرجع *هذا;
	HashConsPool::عملگر=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = بعدی_serial();
	أرجع *هذا;
}

منطقي TwinePool::owns(IdString ref) ثابت {
	أرجع ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ثابت {
	إذا (ref == IdString::Null || ID::is_static(ref))
		أرجع ref;
	أرجع ref.stamped(serial_);
}

فراغ TwinePool::check_owned(IdString ref) ثابت {
	إذا constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ثابت Twine_گره& TwinePool::عملگر[](IdString ref) ثابت {
	check_owned(ref);
	أرجع HashConsPool::عملگر[](ref);
}

اندازه_t TwinePool::gc(pool<IdString>& roots) {
	لكل (خودکار &it : auto_prefixes)
		roots.درج(it.دومی);
	أرجع HashConsPool::gc(roots);
}

ثابت Twine_گره& TwinePool::static_گره(اندازه_t idx) { أرجع StaticTwines::گره(idx); }
فراغ TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

اندازه_t TwinePool::بعدی_serial() {
	ساكن اندازه_t counter = 0;
	اندازه_t serial = ++counter;
	إذا constexpr (IdString::MAX_SERIAL == 0)
		أرجع serial;
	وإلا
		أرجع serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::جمع_کن_inner(Twine_گره t) {
	إذا (free_list.خالی() && STATIC_شمار + backing.اندازه() > IdString::MAX_شاخص)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_شاخص - STATIC_شمار);
	أرجع HashConsPool::جمع_کن_inner(std::move(t));
}

فراغ TwinePool::dump(IdString ref, std::ostream& os) ثابت {
	ثابت Twine_گره& twine = (*هذا)[ref];
	بدل (twine.نوع()) {
	حالة Twine_گره::نوع_2::Dead:
		os << "Dead()";
		اكسر;
	حالة Twine_گره::نوع_2::برگ:
		os << "Leaf(\"" << twine.متن() << "\")";
		اكسر;
	حالة Twine_گره::نوع_2::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.پیشوند_2(), os);
		os << ", tail: \"" << twine.متن() << "\")";
		اكسر;
	}
	إذا (ref.is_عمومی())
		os << " pub";
}

فراغ TwinePool::append_str(IdString ref, std::string& بیرون) ثابت {
	إذا (ref == IdString::Null)
		أرجع;
	إذا (ref.is_عمومی())
		بیرون += '\\';
	ثابت Twine_گره& twine = (*هذا)[ref];
	بدل (twine.نوع()) {
	حالة Twine_گره::نوع_2::Dead:
		اكسر;
	حالة Twine_گره::نوع_2::Suffix:
		append_str(twine.پیشوند_2(), بیرون);
		[[fallthrough]];
	حالة Twine_گره::نوع_2::برگ:
		بیرون += twine.متن();
		اكسر;
	}
}

std::string TwinePool::str(IdString ref) ثابت {
	std::string بیرون;
	append_str(ref, بیرون);
	أرجع بیرون;
}

std::string TwinePool::unescaped_str(IdString ref) ثابت {
	أرجع str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t پیشوند_2, std::string_view متن) ثابت {
	uint32_t hash = پیشوند_2 == Twine_گره::NO_پیشوند ? 0 : (*هذا)[IdString(پیشوند_2)].hash();
	أرجع Twine_گره::extend_hash(hash, متن);
}

IdString TwinePool::پیدا_کن_content(uint32_t پیشوند_2, std::string_view متن) ثابت {
	أرجع پیدا_کن_content(پیشوند_2, متن, content_hash(پیشوند_2, متن));
}

IdString TwinePool::intern(uint32_t پیشوند_2, std::string_view متن) {
	log_assert(!Multithreading::فعال());
	uint32_t hash = content_hash(پیشوند_2, متن);
	IdString ref = پیدا_کن_content(پیشوند_2, متن, hash);
	إذا (ref != IdString::Null)
		أرجع ref;
	أرجع جمع_کن_inner(Twine_گره{پیشوند_2, متن, hash});
}

IdString TwinePool::پیدا_کن(std::string_view name) ثابت {
	إذا (name.خالی())
		أرجع IdString::Null;
	check_name(name);
	خودکار [content, is_عمومی_2] = unescape_name(name);
	أرجع stamp(پیدا_کن_content(Twine_گره::NO_پیشوند, content).برچسب(is_عمومی_2));
}

IdString TwinePool::پیدا_کن(TwineSpec t) ثابت {
	إذا (std::holds_alternative<TwineSpec::AutoSuffix>(t.داده))
		أرجع stamp(پیدا_کن_content(Twine_گره::NO_پیشوند, t.content_str()));
	إذا (خودکار *برگ_2 = std::get_if<TwineSpec::برگ>(&t.داده))
		أرجع stamp(پیدا_کن_content(Twine_گره::NO_پیشوند, برگ_2->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.داده);
	أرجع stamp(پیدا_کن_content((uint32_t)sfx.پیشوند_2.untag().raw(), sfx.دم).برچسب(sfx.پیشوند_2.is_عمومی()));
}

IdString TwinePool::جمع_کن(TwineSpec t) {
	إذا (خودکار *ap = std::get_if<TwineSpec::AutoSuffix>(&t.داده))
		أرجع جمع_کن(auto_پیشوند(ap->پیشوند_2), ap->دم);
	إذا (خودکار *برگ_2 = std::get_if<TwineSpec::برگ>(&t.داده))
		أرجع stamp(intern(Twine_گره::NO_پیشوند, برگ_2->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.داده);
	أرجع جمع_کن(sfx.پیشوند_2, sfx.دم);
}

IdString TwinePool::auto_پیشوند(ثابت std::string *پیشوند_2) {
	خودکار it = auto_prefixes.پیدا_کن(پیشوند_2);
	إذا (it != auto_prefixes.انتها())
		أرجع it->دومی;
	IdString ref = intern(Twine_گره::NO_پیشوند, *پیشوند_2);
	auto_prefixes[پیشوند_2] = ref;
	أرجع ref;
}

IdString TwinePool::جمع_کن(IdString پیشوند_2, std::string_view دم) {
	أرجع stamp(intern((uint32_t)پیشوند_2.untag().raw(), دم).برچسب(پیشوند_2.is_عمومی()));
}

IdString TwinePool::جمع_کن(std::string_view s) {
	إذا (s.خالی())
		أرجع IdString::Null;
	check_name(s);
	خودکار [content, is_عمومی_2] = unescape_name(s);
	أرجع stamp(intern(Twine_گره::NO_پیشوند, content).برچسب(is_عمومی_2));
}

IdString TwinePool::copy_from(ثابت TwinePool& src, IdString ref) {
	IdString یافت = پیدا_کن_from(src, ref);
	إذا (یافت != IdString::Null || ref == IdString::Null)
		أرجع یافت;
	ثابت Twine_گره& t = src[ref.untag()];
	إذا (t.is_dead())
		أرجع IdString::Null;
	uint32_t پیشوند_2 = Twine_گره::NO_پیشوند;
	إذا (t.is_suffix())
		پیشوند_2 = (uint32_t)copy_from(src, t.پیشوند_2()).untag().raw();
	أرجع stamp(intern(پیشوند_2, t.متن()).برچسب(ref.is_عمومی()));
}

std::string TwinePool::ref_token(IdString ref) ثابت {
	أرجع "#" + std::مقصد_string((uint64_t)serial_) + ":"
		+ std::مقصد_string((uint64_t)stamp(ref).بیتها());
}

IdString TwinePool::ref_from_token(std::string_view token) ثابت {
	إذا (token.اندازه() < 2 || token[0] != '#')
		أرجع IdString::Null;
	اندازه_t sep = token.پیدا_کن(':');
	إذا (sep == std::string_view::npos)
		أرجع IdString::Null;
	اندازه_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	لكل (صحيح i = 0; i < 2; i++) {
		إذا (parts[i].خالی())
			أرجع IdString::Null;
		لكل (حرف c : parts[i]) {
			إذا (c < '0' || c > '9')
				أرجع IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	إذا (fields[0] != serial_)
		أرجع IdString::Null;
	IdString ref(fields[1]);
	إذا (ref == IdString::Null)
		أرجع IdString::Null;
	أرجع is_زنده(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
بنية TwineSegments {
	ساكن constexpr اندازه_t SEGMENT_INLINE_عمق = 8;
	TwineSegments(ثابت TwinePool &pool, IdString ref);
	TwineSegments(ثابت TwinePool &pool, IdString سر, std::string_view دم);
	std::string_view peek();
	فراغ advance(اندازه_t n);
	اندازه_t مجموع_اندازه() ثابت;
خاص:
	فراغ افزودن(std::string_view seg);
	std::string_view *segs();
	ثابت std::string_view *segs() ثابت;
	std::string_view inline_segs[SEGMENT_INLINE_عمق];
	std::وکتور<std::string_view> spill;
	اندازه_t شمار = 0;
	اندازه_t pos = 0;
	اندازه_t off = 0;
};

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString ref)
{
	إذا (ref == IdString::Null)
		أرجع;
	لكل (IdString cur = ref.untag(); ;) {
		ثابت Twine_گره &t = pool[cur];
		بدل (t.نوع()) {
		حالة Twine_گره::نوع_2::Suffix:
			افزودن(t.متن());
			cur = t.پیشوند_2();
			استمر;
		حالة Twine_گره::نوع_2::برگ:
			افزودن(t.متن());
			اكسر;
		حالة Twine_گره::نوع_2::Dead:
			اكسر;
		}
		اكسر;
	}
	إذا (ref.is_عمومی())
		افزودن("\\");
	std::معکوس(segs(), segs() + شمار);
}

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString سر, std::string_view دم)
	: TwineSegments(pool, سر)
{
	افزودن(دم);
}

std::string_view TwineSegments::peek()
{
	طالما (pos < شمار) {
		std::string_view seg = segs()[pos];
		إذا (off < seg.اندازه())
			أرجع seg.substr(off);
		pos++;
		off = 0;
	}
	أرجع {};
}

فراغ TwineSegments::advance(اندازه_t n) { off += n; }

اندازه_t TwineSegments::مجموع_اندازه() ثابت
{
	اندازه_t مجموع = 0;
	لكل (اندازه_t i = 0; i < شمار; i++)
		مجموع += segs()[i].اندازه();
	أرجع مجموع;
}

فراغ TwineSegments::افزودن(std::string_view seg)
{
	إذا (spill.خالی() && شمار < SEGMENT_INLINE_عمق) {
		inline_segs[شمار++] = seg;
		أرجع;
	}
	إذا (spill.خالی())
		spill.تخصیص_بده(inline_segs, inline_segs + شمار);
	spill.افزودن_پشت(seg);
	شمار++;
}

std::string_view *TwineSegments::segs() { أرجع spill.خالی() ? inline_segs : spill.داده(); }
ثابت std::string_view *TwineSegments::segs() ثابت { أرجع spill.خالی() ? inline_segs : spill.داده(); }

اندازه_t TwinePool::str_اندازه(IdString ref) ثابت
{
	أرجع TwineSegments(*هذا, ref).مجموع_اندازه();
}

منطقي TwinePool::begins_with(IdString ref, std::string_view پیشوند_2) ثابت
{
	TwineSegments segs(*هذا, ref);
	طالما (!پیشوند_2.خالی()) {
		std::string_view seg = segs.peek();
		إذا (seg.خالی())
			أرجع خطأ;
		اندازه_t n = std::min(seg.اندازه(), پیشوند_2.اندازه());
		إذا (std::memcmp(seg.داده(), پیشوند_2.داده(), n) != 0)
			أرجع خطأ;
		segs.advance(n);
		پیشوند_2.remove_پیشوند(n);
	}
	أرجع درست;
}

منطقي TwinePool::name_equal(IdString ref, std::string_view name) ثابت
{
	أرجع str_اندازه(ref) == name.اندازه() && begins_with(ref, name);
}

ساكن صحيح compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	طالما (درست) {
		std::string_view x = sa.peek(), y = sb.peek();
		إذا (x.خالی() || y.خالی())
			أرجع x.خالی() ? (y.خالی() ? 0 : -1) : 1;
		اندازه_t n = std::min(x.اندازه(), y.اندازه());
		إذا (صحيح diff = std::memcmp(x.داده(), y.داده(), n); diff != 0)
			أرجع diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::پیدا_کن_content(uint32_t پیشوند_2, std::string_view متن, uint32_t hash) ثابت {
	أرجع پیدا_کن_hashed(hash, [&](IdString ref) {
		ثابت Twine_گره &t = (*هذا)[ref];
		إذا (t == Twine_گره::کلید_2{پیشوند_2, متن})
			أرجع درست;
		IdString سر = پیشوند_2 == Twine_گره::NO_پیشوند ? IdString::Null : IdString(پیشوند_2);
		TwineSegments sa(*هذا, ref), sb(*هذا, سر, متن);
		أرجع compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::پیدا_کن_from(ثابت TwinePool& src, IdString ref) ثابت {
	إذا (ref == IdString::Null || ID::is_static(ref))
		أرجع ref;
	IdString idx = ref.untag();
	ثابت Twine_گره& t = src[idx];
	إذا (t.is_dead())
		أرجع IdString::Null;
	IdString یافت = پیدا_کن_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*هذا, cand), sb(src, idx);
		أرجع compare_segments(sa, sb) == 0;
	});
	أرجع stamp(یافت.برچسب(ref.is_عمومی()));
}

IdString TwinePool::پیشوند_از(IdString ref) ثابت {
	ثابت Twine_گره &t = (*هذا)[ref];
	إذا (!t.is_suffix())
		أرجع IdString::Null;
	أرجع stamp(t.پیشوند_2().برچسب(ref.is_عمومی()));
}

صحيح TwinePool::compare_by_name(IdString a, IdString b) ثابت
{
	إذا (a == b)
		أرجع 0;
	إذا (a == IdString::Null)
		أرجع -1;
	إذا (b == IdString::Null)
		أرجع 1;

	إذا (a.is_عمومی() == b.is_عمومی()) {
		ثابت Twine_گره &ta = (*هذا)[a.untag()];
		ثابت Twine_گره &tb = (*هذا)[b.untag()];
		إذا (ta.is_برگ() && tb.is_برگ())
			أرجع ta.متن().compare(tb.متن());
		إذا (ta.is_suffix() && tb.is_suffix() && ta.پیشوند_2() == tb.پیشوند_2())
			أرجع ta.متن().compare(tb.متن());
	}

	TwineSegments sa(*هذا, a), sb(*هذا, b);
	أرجع compare_segments(sa, sb);
}

YOSYS_NAMESPACE_انتها
