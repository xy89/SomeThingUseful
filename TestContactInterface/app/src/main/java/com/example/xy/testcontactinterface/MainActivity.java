package com.example.xy.testcontactinterface;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.ContactsContract;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;


public class MainActivity extends Activity {

    private static final String[] PROJECTION = {ContactsContract.Contacts._ID,
            ContactsContract.Contacts.LOOKUP_KEY,
            ContactsContract.Contacts.DISPLAY_NAME};

    private static final String[] PROJECTION2 = {ContactsContract.CommonDataKinds.Phone.NUMBER};

    private static final String SELECTION = ContactsContract.Contacts.DISPLAY_NAME + " = ?";
    private static final String SELECTION2 = ContactsContract.Data.LOOKUP_KEY + " = ?" +
            " AND " + ContactsContract.Data.MIMETYPE + " = " +
            "'" + ContactsContract.CommonDataKinds.Phone.CONTENT_ITEM_TYPE + "'";



    private static final int CONTACT_ID_INDEX = 0;
    private static final int LOOKUP_KEY_INDEX = 1;
    private static final int DISPLAY_NAME = 2;

    private TextView textView;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button openDialer = (Button) findViewById(R.id.openDialer);
        Button openContact = (Button) findViewById(R.id.openContact);
        Button openSms = (Button) findViewById(R.id.openSms);
        textView = (TextView)findViewById(R.id.textView);


        openDialer.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // with phone number
                //Uri uri = Uri.parse("tel:10086");
                //Intent intent = new Intent(Intent.ACTION_DIAL, uri);
                //startActivity(intent);

                //without phone number
                Intent intent = new Intent(Intent.ACTION_DIAL);
                startActivity(intent);
            }
        });

        openContact.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //search contact by pinyin
                //ContentResolver resolver = getContentResolver();
                //Uri contentUri = Uri.withAppendedPath(ContactsContract.Contacts.CONTENT_FILTER_URI, Uri.encode("zhangsan"));
                //Cursor cursor = resolver.query(contentUri, PROJECTION, null, null, null);

                //search contact by name
                ContentResolver resolver = getContentResolver();
                Cursor cursor = resolver.query(ContactsContract.Contacts.CONTENT_URI, PROJECTION, SELECTION, new String[]{"张三"},null);
                if (cursor != null && cursor.moveToFirst()) {
                    long id = cursor.getLong(CONTACT_ID_INDEX);
                    String key = cursor.getString(LOOKUP_KEY_INDEX);
                    //search phone number
                    Cursor cursor2 = resolver.query(ContactsContract.Data.CONTENT_URI, PROJECTION2, SELECTION2, new String[]{key}, null);
                    if (cursor2 != null && cursor2.moveToFirst()) {
                        String phoneNumber = cursor2.getString(0);
                        textView.setText(phoneNumber);
                    }
                    cursor2.close();

                    //Uri uri = ContactsContract.Contacts.getLookupUri(id, key);
                    //got to contact detail page
                    //Intent  intent = new Intent(Intent.ACTION_VIEW, mUri);
                }
                cursor.close();

                //go to contact search page
                //Intent intent = new Intent(ContactsContract.Intents.UI.FILTER_CONTACTS_ACTION);
                //intent.setType(ContactsContract.Contacts.CONTENT_TYPE);
                //intent.putExtra(ContactsContract.Intents.UI.FILTER_TEXT_EXTRA_KEY, "zhangsan");
                //startActivity(intent);


            }
        });

        openSms.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //without message and phone number
                //Intent intent = new Intent(Intent.ACTION_SENDTO);
                //intent.setData(Uri.parse("smsto:"));
                //startActivity(intent);

                //with message and phone number
                Intent intent = new Intent(Intent.ACTION_SENDTO);
                intent.setData(Uri.parse("smsto:10086"));
                intent.putExtra("sms_body", "hello");
                startActivity(intent);
            }
        });
    }
}
